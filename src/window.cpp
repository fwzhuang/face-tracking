//raytracer.mustafaisik.net//

#include "window.h"
#include "util.h"

#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

Window::Window(int gui_width, int screen_width, int screen_height)
	: m_gui_width(gui_width)
	, m_screen_width(screen_width)
	, m_screen_height(screen_height)
{
	//Initialize GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("GLFW is not initialized properly");
	}

	//OpenGL 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(m_screen_width + m_gui_width, m_screen_height, "Face2Face", nullptr, nullptr);

	if (!m_window)
	{
		throw std::runtime_error("GLFW could not create the window");
	}
	glfwMakeContextCurrent(m_window);

	//Initialize GLAD
	if (!gladLoadGL())
	{
		throw std::runtime_error("GLAD is not initialized properly");
	}

	//OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(m_gui_width, 0, m_screen_width, m_screen_height);

	//ImGui inits.
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(nullptr);
	io.Fonts->AddFontDefault();
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
	ImGui::DestroyContext();
}

//Call this function after you're done with all your draw calls.
void Window::refresh()
{
	glfwSwapBuffers(m_window);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::queryKey(int key, int condition) const
{
	return glfwGetKey(m_window, key) == condition;
}

void Window::setWindowTitle(const std::string& title) const
{
	glfwSetWindowTitle(m_window, title.c_str());
}

void Window::getCursorPosition(double& x, double& y) const
{
	glfwGetCursorPos(m_window, &x, &y);
}