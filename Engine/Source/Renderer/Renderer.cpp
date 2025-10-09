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

		background_color = RgbToGlmVec4(22, 22, 22, 1.0f);
		viewport_framebuffer = std::make_unique<Framebuffer>(viewport_size.x, viewport_size.y);
		console_capture = std::make_unique<ConsoleCapture>();
		console_capture->StartCapture();
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		// LOAD FONTS
		ImGuiIO& io = ImGui::GetIO();
		font_title = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Space_Mono/SpaceMono-Regular.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		font_body = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Space_Mono/SpaceMono-Regular.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesDefault());

		// --- FOR TESTING - REMOVE AFTER ADDING SUPPORT IN ENGINE ---
		default_shader = std::make_unique<Shader>("Default", "Resources/Shaders/default.vert", "Resources/Shaders/default.frag", "None");

		wood_floor_texture = std::make_shared<Texture>("Resources/Textures/wood_floor.png", DIFFUSE);
		checkered_texture = std::make_shared<Texture>("Resources/Textures/checkered.png", DIFFUSE);

		std::shared_ptr<Entity> test_sphere_entity2 = std::make_shared<Entity>("Test Sphere Entity");
		test_sphere_entity2->AddComponent<Transform>();
		test_sphere_entity2->AddComponent<Model>("Resources/Models/Sphere.obj");
		test_sphere_entity2->AddComponent<Textures>();
		test_sphere_entity2->GetComponent<Textures>()->AddTexture(checkered_texture);

		std::shared_ptr<Entity> test_cube_entity = std::make_shared<Entity>("Test Cube Entity");
		test_cube_entity->AddComponent<Transform>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(5.0f, 0.25f, 5.0f));
		test_cube_entity->AddComponent<Model>("Resources/Models/Cube.obj");
		test_cube_entity->AddComponent<Textures>();
		test_cube_entity->GetComponent<Textures>()->AddTexture(wood_floor_texture);
		test_cube_entity->children.push_back(test_sphere_entity2);
		test_cube_entity->children[0]->parent = test_cube_entity;

		std::shared_ptr<Entity> test_sphere_entity = std::make_shared<Entity>("Test Sphere Entity");
		test_sphere_entity->AddComponent<Transform>();
		test_sphere_entity->AddComponent<Model>("Resources/Models/Sphere.obj");
		test_sphere_entity->AddComponent<Textures>();
		test_sphere_entity->GetComponent<Textures>()->AddTexture(checkered_texture);
		test_sphere_entity->children.push_back(test_cube_entity);
		test_sphere_entity->children[0]->parent = test_sphere_entity;
		
		std::shared_ptr<Entity> test_cube_entity2 = std::make_shared<Entity>("Test Cube Entity");
		test_cube_entity2->AddComponent<Transform>(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(5.0f, 0.25f, 5.0f));
		test_cube_entity2->AddComponent<Model>("Resources/Models/Cube.obj");
		test_cube_entity2->AddComponent<Textures>();
		test_cube_entity2->GetComponent<Textures>()->AddTexture(wood_floor_texture);
		
		entities.push_back(test_cube_entity);
		entities.push_back(test_sphere_entity);
		entities.push_back(test_sphere_entity2);
		entities.push_back(test_cube_entity2);
		
		default_shader->Use();
		default_shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));

		current_entity = test_cube_entity;
		// -------

		for (auto entity : entities)
		{
			entity->LoadComponents();
		}
	}
	void Renderer::OnDetach()
	{
		console_capture->StopCapture();
	}

	void Renderer::OnUpdate()
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		GLFWwindow* glfwWindow = window.GetNativeWindow();
		const float deltaTime = project.GetDeltaTime();

		// --- TESTING - IMPROVE IMPLEMENTATION AT LATER TIME ---
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
		// ---

		viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = engine_camera->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		glm::mat4 view = engine_camera->GetViewMatrix();

		default_shader->Use();
		default_shader->SetMat4("projection", projection);
		default_shader->SetMat4("view", view);

		for (const std::shared_ptr<Entity>& entity : entities)
		{
			entity->Draw(*default_shader, manipulation_matrix);
		}

		viewport_framebuffer->Unbind();
		glViewport(0, 0, window.GetWidth(), window.GetHeight());

		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::OnInterfaceUpdate()
	{
		Project& project = Project::GetInstance();
		Interface& project_interface = project.GetInterface();
		Window& project_window = project.GetWindow();
		ImVec4& highlight_color = project_interface.highlight_primary;
		
		// -- VIEWPORT --
		ImGui::PushFont(font_title);
		ImGui::Begin("Viewport");
		DrawActiveTitleLine(highlight_color);
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::SetWindowFocus();
			engine_camera_can_rotate = true;
			glfwSetInputMode(project_window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		viewport_focused = ImGui::IsWindowFocused();
		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		
		if (viewport_panel_size.x != viewport_size.x || viewport_panel_size.y != viewport_size.y)
		{
			if (viewport_panel_size.x > 0 && viewport_panel_size.y > 0)
			{
				viewport_size = {viewport_panel_size.x, viewport_panel_size.y};
				viewport_framebuffer->Resize(viewport_size.x, viewport_size.y);
			}
		}

		ImGui::PopFont();

		ImGui::Image((void*)(intptr_t)viewport_framebuffer->GetColorAttachment(), viewport_panel_size, ImVec2(0,1), ImVec2(1, 0));
		ImGui::End();

		// -- PROJECT SETTINGS --
		ImGui::PushFont(font_title);
		ImGui::Begin("Project Settings", nullptr);
		DrawActiveTitleLine(highlight_color);
		ImGui::Indent(8.0f); // Add left padding for content
		ImGui::Spacing(); // Add top spacing
		ImGui::PopFont();

		std::string frame_count = "Frame " + std::to_string(project.GetFrameCount());
		std::string delta_time = "Delta Time: " + std::to_string(project.GetDeltaTime());
		
		ImGui::PushFont(font_body);
		ImGui::Text(frame_count.c_str());
		ImGui::Text(delta_time.c_str());
		ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("DragStep", &drag_step, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();
		ImGui::PopFont();
		
		ImGui::Unindent(8.0f);
		ImGui::End();

		// -- HIERARCHY --
		ImGui::PushFont(font_title);
		ImGui::Begin("Hierarchy", nullptr);
		DrawActiveTitleLine(highlight_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();

		ImGui::PushFont(font_body);
		ImGui::PushStyleColor(ImGuiCol_Header, project_interface.background_primary);
		for (auto entity : entities)
		{
			if (entity->parent == nullptr)
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				ImGuiTreeNodeFlags child_flags = flags | ImGuiTreeNodeFlags_Leaf;

				if (entity->children.empty())
					flags = child_flags;
					
				if (entity == current_entity)
					flags |= ImGuiTreeNodeFlags_Selected;
					
    
				bool node_open = ImGui::TreeNodeEx(entity->name.c_str(), flags);
    
				if (ImGui::IsItemClicked())
					current_entity = entity;
    
				if (node_open)
				{
					DisplayChildrenFromParent(entity);
					ImGui::TreePop();
				}
			}
		}
		ImGui::PopStyleColor();
		ImGui::PopFont();
		
		ImGui::Unindent(8.0f);
		ImGui::End();

		// -- DETAILS --
		ImGui::PushFont(font_title);
		ImGui::Begin("Details", nullptr);
		DrawActiveTitleLine(highlight_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		
		std::shared_ptr<Transform> current_entity_transform = current_entity->GetComponent<Transform>();
		
		ImGui::PushFont(font_body);
		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputText(" ", &current_entity->name);
		
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::PushItemWidth(200.0f);
			ImGui::DragFloat3("Position ", (float*)&current_entity_transform->position, drag_step, -1000, 1000);
			ImGui::DragFloat3("Scale ", (float*)&current_entity_transform->scale, drag_step, 0, 1000);
			ImGui::DragFloat3("Rotation ", (float*)&current_entity_transform->rotation, drag_step, 0, 360);
			ImGui::PopItemWidth();
		}
		ImGui::PopFont();

		ImGui::Unindent(8.0f);
		ImGui::End();

		// -- CONSOLE --
		ImGui::PushFont(font_title);
		ImGui::Begin("Console", nullptr);
		DrawActiveTitleLine(highlight_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();

		ImGui::PushFont(font_body);
		std::vector<std::string> lines = console_capture->GetLines();
		for (const std::string& line : lines)
		{
			auto [color, text] = ParseAnsiLine(line);
			ImGui::TextColored(color, "%s", text.c_str());
		}
		ImGui::PopFont();

		ImGui::Unindent(8.0f);
		ImGui::End();
	}

	// Used for engine input, not game logic input
	void Renderer::OnInput(Input& input)
	{
		Project& project = Project::GetInstance();
		Window& window = project.GetWindow();
		GLFWwindow* glfw_window = window.GetNativeWindow();

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
					if (viewport_focused)
					{
						engine_camera_can_rotate = true;
						glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					}
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
					glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

	void Renderer::DisplayChildrenFromParent(std::shared_ptr<Entity> parent)
	{
		for (auto child : parent->children)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        
			if (child->children.empty())
				flags |= ImGuiTreeNodeFlags_Leaf;
            
			if (child == current_entity)
				flags |= ImGuiTreeNodeFlags_Selected;
        
			bool node_open = ImGui::TreeNodeEx(child->name.c_str(), flags);
        
			if (ImGui::IsItemClicked())
				current_entity = child;

			if (node_open)
			{
				DisplayChildrenFromParent(child);
				ImGui::TreePop();
			}
		}
	}

	
	void Renderer::DrawActiveTitleLine(const ImVec4& color, float thickness)
	{
		if (!ImGui::IsWindowFocused())
			return;
    
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		ImVec2 window_pos = ImGui::GetWindowPos();
    
		const char* title = ImGui::GetCurrentWindow()->Name;
		ImVec2 text_size = ImGui::CalcTextSize(title);
		float title_padding = ImGui::GetStyle().FramePadding.x;
		float title_x = window_pos.x + title_padding * 3.5f;
    
		draw_list->AddRectFilled(
			ImVec2(window_pos.x + title_padding * 1.25f, window_pos.y),
			ImVec2(title_x + text_size.x, window_pos.y + thickness),
			ImGui::ColorConvertFloat4ToU32(color)
		);
	}
}
