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
		engine_camera_can_rotate = false;
		
		default_shader = std::make_unique<Shader>("Default", "Resources/Shaders/default.vert", "Resources/Shaders/default.frag", "None");

		wood_floor_texture = std::make_shared<Texture>("Resources/Textures/wood_floor.png", DIFFUSE);
		checkered_texture = std::make_shared<Texture>("Resources/Textures/checkered.png", DIFFUSE);
		
		cube_model = std::make_shared<Model>("Resources/Models/Cube.obj");
		sphere_model = std::make_shared<Model>("Resources/Models/Sphere.obj");

		cube_model->AddTexture(wood_floor_texture);
		sphere_model->AddTexture(checkered_texture);
		
		cube_model->Load();
		sphere_model->Load();

		std::shared_ptr<Entity> test_cube_entity = std::make_shared<Entity>("Test Cube Entity");
		test_cube_entity->AddComponent<Transform>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(5.0f, 0.25f, 5.0f));
		test_cube_entity->AddComponent<ModelData>(cube_model);

		std::shared_ptr<Entity> test_sphere_entity = std::make_shared<Entity>("Test Sphere Entity");
		test_sphere_entity->AddComponent<Transform>();
		test_sphere_entity->AddComponent<ModelData>(sphere_model);
		
		entities.push_back(test_cube_entity);
		entities.push_back(test_sphere_entity);
		
		default_shader->Use();
		default_shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
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

		default_shader->Use();
		default_shader->SetMat4("projection", projection);
		default_shader->SetMat4("view", view);

		for (const std::shared_ptr<Entity>& entity : entities)
		{
			entity->Draw(*default_shader, manipulation_matrix);
		}
	}

	void Renderer::OnInterfaceUpdate()
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		
		ImGui::SetNextWindowSize(ImVec2(window.GetWidth() / 4, window.GetHeight() / 4), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
		ImGui::Begin("Hierarchy", nullptr);
		ImGui::End();
		
		ImGui::SetNextWindowSize(ImVec2(window.GetWidth() / 4, window.GetHeight() / 4), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(0, window.GetHeight() / 4), ImGuiCond_Once);
		ImGui::Begin("Properties", nullptr);
		ImGui::End();
	}

	// Used for engine input, not game logic input
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

				// -- actions here --

				// maximize window
				if (keyInput.GetKeyCode() == InputCode::F11)
				{
					glfwMaximizeWindow(window.GetNativeWindow());
				}
				
				break;
			}
		case InputType::KeyReleased:
			{
				const auto keyInput = dynamic_cast<KeyReleasedInput&>(input);

				// -- actions here --
				
				break;
			}
		case InputType::KeyTyped:
			{
				const auto keyInput = dynamic_cast<KeyTypedInput&>(input);

				// -- actions here --

				break;
			}
		case InputType::MouseButtonPressed:
			{
				const auto mouseInput = dynamic_cast<MouseButtonPressedInput&>(input);
				
				// -- actions here --

				// enable engine camera rotation
				if (mouseInput.GetMouseButton() == InputCode::Button1)
				{
					engine_camera_can_rotate = true;
					glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				
				break;
			}
		case InputType::MouseButtonReleased:
			{
				const auto mouseInput = dynamic_cast<MouseButtonReleasedInput&>(input);
				
				// -- actions here --

				// disable engine camera rotation
				if (mouseInput.GetMouseButton() == InputCode::Button1)
				{
					engine_camera_can_rotate = false;
					glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				
				break;
			}
		case InputType::MouseMoved:
			{
				const auto mouseInput = dynamic_cast<MouseMovedInput&>(input);

				// -- actions here --

				// move engine camera based on mouse position and movement
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

				if (engine_camera_can_rotate)
					engine_camera->ProcessMouseMovement(x_offset, y_offset);
				
				break;
			}
		case InputType::MouseScrolled:
			{
				const auto mouseInput = dynamic_cast<MouseScrolledInput&>(input);

				// -- actions here --
				
				break;
			}
		case InputType::None:
				break;
		}
	}
}
