#include "application.h"

#include <imgui.h>
#include <glm/ext/matrix_clip_space.inl>
#include <glm/ext/matrix_transform.inl>
#include <utility>

constexpr int kScreenWidth = 1440;
constexpr int kScreenHeight = 900;
constexpr glm::ivec2 kGuiPosition(0, 0);
constexpr glm::ivec2 kGuiSize(240, kScreenHeight);

static std::string kMorphableModelPath("../MorphableModel/");

Application::Application()
	: m_window(kGuiSize.x, kScreenWidth, kScreenHeight)
	, m_face(std::make_shared<Face>(kMorphableModelPath))
	, m_solver(m_face)
	, m_tracker()
	, m_menu(kGuiPosition, kGuiSize)
	, m_camera(0)
{}

void Application::run()
{
	initMenuWidgets();
	initFaceShader();

	while (!glfwWindowShouldClose(m_window.getGLFWWindow()))
	{
		glfwPollEvents();

		m_face->computeFace();

		drawFace();
		m_menu.draw();
		m_window.refresh();

		cv::Mat frame;
		if (!m_camera.read(frame))
		{
			continue;
		}

		auto correspondences = m_tracker.getCorrespondences(frame);

		m_solver.solve(correspondences);
	}
}

void Application::initMenuWidgets()
{
	auto& shape_coefficients = m_face->getShapeCoefficients();
	auto shape_parameters_gui = [&shape_coefficients]()
	{
		ImGui::CollapsingHeader("Shape Parameters", ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SliderFloat("Shape1", &shape_coefficients[0], -5.0f, 5.0f);
		ImGui::SliderFloat("Shape2", &shape_coefficients[1], -5.0f, 5.0f);
		ImGui::SliderFloat("Shape3", &shape_coefficients[2], -5.0f, 5.0f);
		ImGui::SliderFloat("Shape4", &shape_coefficients[3], -5.0f, 5.0f);
		ImGui::SliderFloat("Shape5", &shape_coefficients[4], -5.0f, 5.0f);
	};
	m_menu.attach(std::move(shape_parameters_gui));

	auto& albedo_coefficients = m_face->getAlbedoCoefficients();
	auto albedo_parameters_gui = [&albedo_coefficients]()
	{
		ImGui::CollapsingHeader("Albedo Parameters", ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SliderFloat("Albedo1", &albedo_coefficients[0], -5.0f, 5.0f);
		ImGui::SliderFloat("Albedo2", &albedo_coefficients[1], -5.0f, 5.0f);
		ImGui::SliderFloat("Albedo3", &albedo_coefficients[2], -5.0f, 5.0f);
		ImGui::SliderFloat("Albedo4", &albedo_coefficients[3], -5.0f, 5.0f);
		ImGui::SliderFloat("Albedo5", &albedo_coefficients[4], -5.0f, 5.0f);
	};
	m_menu.attach(std::move(albedo_parameters_gui));

	auto& expression_coefficients = m_face->getExpressionCoefficients();
	auto expression_parameters_gui = [&expression_coefficients]()
	{
		ImGui::CollapsingHeader("Expression Parameters", ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SliderFloat("Expression1", &expression_coefficients[0], 0.0f, 1.0f);
		ImGui::SliderFloat("Expression2", &expression_coefficients[1], 0.0f, 1.0f);
		ImGui::SliderFloat("Expression3", &expression_coefficients[2], 0.0f, 1.0f);
		ImGui::SliderFloat("Expression4", &expression_coefficients[3], 0.0f, 1.0f);
		ImGui::SliderFloat("Expression5", &expression_coefficients[4], 0.0f, 1.0f);
	};
	m_menu.attach(std::move(expression_parameters_gui));

	auto& sh_coefficients = m_face->getSHCoefficients();
	auto sh_parameters_gui = [&sh_coefficients]()
	{
		ImGui::CollapsingHeader("Spherical Harmonics Parameters", ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::SliderFloat("Ambient", &sh_coefficients[0], -1.0f, 1.0f);
		ImGui::SliderFloat("y", &sh_coefficients[1], -1.0f, 1.0f);
		ImGui::SliderFloat("z", &sh_coefficients[2], -1.0f, 1.0f);
		ImGui::SliderFloat("x", &sh_coefficients[3], -1.0f, 1.0f);
		ImGui::SliderFloat("xy", &sh_coefficients[4], -1.0f, 1.0f);
		ImGui::SliderFloat("yz", &sh_coefficients[5], -1.0f, 1.0f);
		ImGui::SliderFloat("3z2 - 1", &sh_coefficients[6], -1.0f, 1.0f);
		ImGui::SliderFloat("xz", &sh_coefficients[7], -1.0f, 1.0f);
		ImGui::SliderFloat("x2-y2", &sh_coefficients[8], -1.0f, 1.0f);
	};
	m_menu.attach(std::move(sh_parameters_gui));

	auto gpu_memory_info_gui = []()
	{
		ImGui::Separator();

		size_t free, total;
		CHECK_CUDA_ERROR(cudaMemGetInfo(&free, &total));
		ImGui::Text("Free  GPU Memory: %.1f MB", free / (1024.0f * 1024.0f));
		ImGui::Text("Total GPU Memory: %.1f MB", total / (1024.0f * 1024.0f));
		ImGui::End();
	};
	m_menu.attach(std::move(gpu_memory_info_gui));
}

void Application::initFaceShader()
{
	m_face_shader.attachShader(GL_VERTEX_SHADER, "../src/shader/face.vert");
	m_face_shader.attachShader(GL_FRAGMENT_SHADER, "../src/shader/face.frag");
	m_face_shader.link();

	//TODO: When we implement optimization, model parameters should be inside the Face class.
	const auto model = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	//TODO: When we implement optimization, camera pose and camera intrinsics should be defined in a camera class or another container.
	const auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const auto projection = glm::perspective(45.0f, static_cast<float>(kScreenWidth) / kScreenHeight, 0.01f, 100.0f);

	m_face_shader.use();
	m_face_shader.setMat4("model", model);
	m_face_shader.setMat4("view", view);
	m_face_shader.setMat4("projection", projection);
}

void Application::drawFace()
{
	m_face_shader.use();
	m_face_shader.setUniformFVVar("sh_coefficients", m_face->getSHCoefficients());

	m_face->updateVertexBuffer();
	m_face->draw(m_face_shader);
}