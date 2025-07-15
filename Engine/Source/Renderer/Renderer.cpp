#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
	Renderer::Renderer()
	{
		m_ProjectPath = std::filesystem::current_path().generic_string();
	}
	Renderer::~Renderer()
	{

	}

	void Renderer::OnAttach()
	{
		m_ManipulationMatrix = new glm::mat4(1.0f);
		m_EngineCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

		std::vector<Vertex> test_vertices = {
			// Front face
			{{-0.5f, -0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom Left
			{{ 0.5f, -0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom Right
			{{ 0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top Right
			{{-0.5f,  0.5f,  0.5f},  {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top Left

			// Back face
			{{ 0.5f, -0.5f, -0.5f},  {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
			{{-0.5f, -0.5f, -0.5f},  {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
			{{-0.5f,  0.5f, -0.5f},  {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
			{{ 0.5f,  0.5f, -0.5f},  {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},

			// Left face
			{{-0.5f, -0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
			{{-0.5f, -0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
			{{-0.5f,  0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
			{{-0.5f,  0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

			// Right face
			{{ 0.5f, -0.5f,  0.5f},  {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
			{{ 0.5f, -0.5f, -0.5f},  {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
			{{ 0.5f,  0.5f, -0.5f},  {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
			{{ 0.5f,  0.5f,  0.5f},  {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},

			// Top face
			{{-0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
			{{ 0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
			{{ 0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
			{{-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},

			// Bottom face
			{{-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
			{{ 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
			{{ 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
			{{-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
		};
		std::vector<GLuint> test_indices = {
			0, 1, 2,  2, 3, 0,        // Front
			4, 5, 6,  6, 7, 4,        // Back
			8, 9,10, 10,11, 8,        // Left
		   12,13,14, 14,15,12,        // Right
		   16,17,18, 18,19,16,        // Top
		   20,21,22, 22,23,20         // Bottom
		};
		std::vector<Texture> test_textures = {};
		
		m_TestMesh = new Mesh(test_vertices, test_indices, test_textures);
		m_TestShader = new Shader("Default", "Resources/Shaders/default.vert", "Resources/Shaders/default.frag", "None");
		
		m_TestShader->Use();
		m_TestShader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
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

		if (glfwGetKey(glfwWindow, InputCode::W) == GLFW_PRESS)
			m_EngineCamera->ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::S) == GLFW_PRESS)
			m_EngineCamera->ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::A) == GLFW_PRESS)
			m_EngineCamera->ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::D) == GLFW_PRESS)
			m_EngineCamera->ProcessKeyboard(RIGHT, deltaTime);
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = m_EngineCamera->GetProjectionMatrix(window.GetWidth(), window.GetHeight());
		glm::mat4 view = m_EngineCamera->GetViewMatrix();

		m_TestShader->Use();
		m_TestShader->SetMat4("projection", projection);
		m_TestShader->SetMat4("view", view);
		m_TestShader->SetMat4("model", glm::mat4(1.0f));

		m_TestMesh->Draw(*m_TestShader);
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

				if (m_EngineCamera->firstMouse)
				{
					m_EngineCamera->lastX = x_position;
					m_EngineCamera->lastY = y_position;
					m_EngineCamera->firstMouse = false;
				}

				const float x_offset = x_position - m_EngineCamera->lastX;
				const float y_offset = y_position - m_EngineCamera->lastY;

				m_EngineCamera->lastX = x_position;
				m_EngineCamera->lastY = y_position;

				m_EngineCamera->ProcessMouseMovement(x_offset, y_offset);
				
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
		}
	}
}
