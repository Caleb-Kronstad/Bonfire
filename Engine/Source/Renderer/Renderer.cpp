#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
	Renderer::Renderer()
	{
		project_path = std::filesystem::current_path().generic_string();
	}
	Renderer::~Renderer()
	{

	}

	void Renderer::OnAttach()
	{
		manipulation_matrix = glm::mat4(1.0f);
		engine_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

		test_texture = std::make_shared<Texture>("Resources/Textures/wood_floor.png", DIFFUSE);
		test_model = std::make_unique<Model>("Resources/Models/Cube.obj");
		test_shader = std::make_unique<Shader>("Default", "Resources/Shaders/default.vert", "Resources/Shaders/default.frag", "None");

		test_model->AddTexture(test_texture);
		test_model->Load();
		
		test_shader->Use();
		test_shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
	}
	void Renderer::OnDetach()
	{
	}

	void Renderer::OnUpdate()
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		GLFWwindow* glfwWindow = window.GetNativeWindow();
		const float deltaTime = project.GetDeltaTime();

		if (window.GetWidth() <= 0 || window.GetHeight() <= 0)
			return;

		if (glfwGetKey(glfwWindow, InputCode::W) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::S) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::A) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::D) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(RIGHT, deltaTime);
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = engine_camera->GetProjectionMatrix(window.GetWidth(), window.GetHeight());
		glm::mat4 view = engine_camera->GetViewMatrix();

		test_shader->Use();
		test_shader->SetMat4("projection", projection);
		test_shader->SetMat4("view", view);
		test_shader->SetMat4("model", manipulation_matrix);

		test_model->Draw(*test_shader);
	}

	void Renderer::OnInterfaceUpdate()
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();

		ImGuiWindowFlags window_flags = 0;
		
		ImGui::SetNextWindowSize(ImVec2(window.GetWidth() / 4, window.GetHeight() / 4), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::Begin("Hierarchy", nullptr, window_flags);
		ImGui::Text("Hierarchy");
		ImGui::End();
		
		ImGui::SetNextWindowSize(ImVec2(window.GetWidth() / 4, window.GetHeight() / 4), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, window.GetHeight() / 4), ImGuiCond_Always);
		ImGui::Begin("Properties", nullptr, window_flags);
		ImGui::Text("Properties");
		ImGui::End();
	}

	void Renderer::OnInput(Input& input)
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		GLFWwindow* glfwWindow = window.GetNativeWindow();

		switch (input.GetInputType())
		{
		case InputType::KeyPressed:
			{
				const auto keyInput = dynamic_cast<KeyPressedInput&>(input);

				// actions here
				
				break;
			}
		case InputType::KeyReleased:
			{
				const auto keyInput = dynamic_cast<KeyReleasedInput&>(input);

				// actions here
				
				break;
			}
		case InputType::KeyTyped:
			{
				const auto keyInput = dynamic_cast<KeyTypedInput&>(input);

				// actions here

				break;
			}
		case InputType::MouseButtonPressed:
			{
				const auto mouseInput = dynamic_cast<MouseButtonPressedInput&>(input);
				
				// actions here
				
				break;
			}
		case InputType::MouseButtonReleased:
			{
				const auto mouseInput = dynamic_cast<MouseButtonReleasedInput&>(input);
				
				// actions here
				
				break;
			}
		case InputType::MouseMoved:
			{
				const auto mouseInput = dynamic_cast<MouseMovedInput&>(input);

				// actions here
				const float x_position = mouseInput.GetX();
				const float y_position = mouseInput.GetY();

				if (engine_camera->firstMouse)
				{
					engine_camera->lastX = x_position;
					engine_camera->lastY = y_position;
					engine_camera->firstMouse = false;
				}

				const float x_offset = x_position - engine_camera->lastX;
				const float y_offset = y_position - engine_camera->lastY;

				engine_camera->lastX = x_position;
				engine_camera->lastY = y_position;

				engine_camera->ProcessMouseMovement(x_offset, y_offset);
				
				break;
			}
		case InputType::MouseScrolled:
			{
				const auto mouseInput = dynamic_cast<MouseScrolledInput&>(input);

				// actions here
				
				break;
			}
		case InputType::None:
				break;
		default:
				break;
		}
	}
}
