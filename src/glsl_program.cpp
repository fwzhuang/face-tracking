#include "glsl_program.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

GLSLProgram::GLSLProgram(GLSLProgram&& rhs)
	: m_program(rhs.m_program)
	, m_shaders(std::move(rhs.m_shaders))
{
	rhs.m_program = 0;
}

GLSLProgram& GLSLProgram::operator=(GLSLProgram&& rhs)
{
	destroyProgram();
	destroyShaders();

	m_program = rhs.m_program;
	m_shaders = std::move(rhs.m_shaders);
	rhs.m_program = 0;

	return *this;
}

GLSLProgram::~GLSLProgram()
{
	destroyProgram();
	destroyShaders();
}

void GLSLProgram::attachShader(GLenum shader_type, const std::string& shader_path)
{
	std::ifstream shader_file;
	std::string shader_code;

	//Ensure that files can throw exceptions.
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream vs_stream;
		shader_file.open(shader_path);

		//Read the whole text from the file.
		vs_stream << shader_file.rdbuf();

		shader_file.close();

		//Load vertex and fragment shaders code from the streams.
		shader_code = vs_stream.str();
	}

	catch (const std::ifstream::failure&)
	{
		std::cout << shader_path << std::endl;
		std::cout << "ERROR::glsl_program.cpp::attachShader::SHADER_FILE_NOT_SUCCESFULLY_READ/OPENED" << std::endl;
	}

	GLuint shader = 0;
	shader = glCreateShader(shader_type);
	assert(shader);

	const GLchar* sc_ptr = shader_code.c_str();
	glShaderSource(shader, 1, &sc_ptr, nullptr);
	glCompileShader(shader);
	m_shaders.push_back(shader);

	//Check errors, if any.
	GLint success;
	GLchar info_log[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, nullptr, info_log);
		std::cout << shader_path << std::endl;
		std::cout << "ERROR::glsl_program.cpp::attachShader::COMPILATION_FAILED\n" << info_log << std::endl;
	}
}

void GLSLProgram::link()
{
	destroyProgram();

	m_program = glCreateProgram();
	assert(m_program);

	for (auto shader : m_shaders)
	{
		glAttachShader(m_program, shader);
	}
	glLinkProgram(m_program);

	destroyShaders();

	//Check errors, if any.
	GLint success;
	GLchar info_log[1024];
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_program, 1024, nullptr, info_log);
		std::cout << "ERROR::glsl_program.cpp::link::LINKING_FAILED\n" << info_log << std::endl;
	}
}

void GLSLProgram::use() const
{
	assert(m_program);
	assert(glIsProgram(m_program));
	glUseProgram(m_program);
}

void GLSLProgram::setUniformIVar(const std::string& name, const std::vector<GLint>& values) const
{
	auto begin = values.begin();

	auto size = values.size();
	assert(size > 0 && size < 5);

	switch (size)
	{
	case 1:
		glUniform1i(glGetUniformLocation(m_program, name.c_str()), begin[0]);
		break;

	case 2:
		glUniform2i(glGetUniformLocation(m_program, name.c_str()), begin[0], begin[1]);
		break;

	case 3:
		glUniform3i(glGetUniformLocation(m_program, name.c_str()), begin[0], begin[1], begin[2]);
		break;

	case 4:
		glUniform4i(glGetUniformLocation(m_program, name.c_str()), begin[0], begin[1], begin[2], begin[3]);
		break;
	}
}

void GLSLProgram::setUniformFVar(const std::string& name, const std::vector<GLfloat>& values) const
{
	auto begin = values.begin();

	auto size = values.size();
	assert(size > 0 && size < 5);

	switch (size)
	{
	case 1:
		glUniform1f(glGetUniformLocation(m_program, name.c_str()), begin[0]);
		break;

	case 2:
		glUniform2f(glGetUniformLocation(m_program, name.c_str()), begin[0], begin[1]);
		break;

	case 3:
		glUniform3f(glGetUniformLocation(m_program, name.c_str()), begin[0], begin[1], begin[2]);
		break;

	case 4:
		glUniform4f(glGetUniformLocation(m_program, name.c_str()), begin[0], begin[1], begin[2], begin[3]);
		break;
	}
}

void GLSLProgram::setUniformFVVar(const std::string& name, const std::vector<GLfloat>& values) const
{
	glUniform1fv(glGetUniformLocation(m_program, name.c_str()), values.size(), values.data());
}

void GLSLProgram::setMat4(const std::string& name, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void GLSLProgram::destroyProgram()
{
	if (m_program)
	{
		glDeleteProgram(m_program);
		m_program = 0;
	}
}

void GLSLProgram::destroyShaders()
{
	for (auto shader : m_shaders)
	{
		glDeleteShader(shader);
	}

	m_shaders.clear();
}