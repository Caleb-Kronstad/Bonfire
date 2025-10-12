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

		param_database = std::make_unique<ParamDatabase>();
		param_database->LoadModelParams("Params/modelparams.bonfireparams");
		param_database->LoadTextureParams("Params/textureparams.bonfireparams");

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
		test_entity_id = EntityID(1000001);
		bool test_entity_enabled = false;
		std::string test_entity_name = "Floor";
		glm::vec3 test_entity_position = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 test_entity_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 test_entity_scale = glm::vec3(5.0f, 0.25f, 5.0f);
		ParamReference model_param_ref = ParamReference(1001);
		ParamReference texture_param_ref = ParamReference(1000);
		test_entity_data = EntityData(test_entity_enabled, test_entity_name, test_entity_position, test_entity_rotation, test_entity_scale);
		test_entity_data.AddParam(PARAM_TYPE::MODEL, model_param_ref);
		test_entity_data.AddParam(PARAM_TYPE::TEXTURE, texture_param_ref);

		test_model = Model("Resources/Models/Cube.obj");
		test_model.Load();
		
		default_shader = std::make_unique<Shader>("Default", "Resources/Shaders/default.vert", "Resources/Shaders/default.frag", "None");
		
		default_shader->Use();
		default_shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));

		current_entity_id = test_entity_id;
		// -------
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
			engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::FORWARD, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::S) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::BACKWARD, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::A) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::LEFT, deltaTime);
		if (glfwGetKey(glfwWindow, InputCode::D) == GLFW_PRESS)
			engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::	RIGHT, deltaTime);
		// ---

		viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = engine_camera->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		glm::mat4 view = engine_camera->GetViewMatrix();

		default_shader->Use();
		default_shader->SetMat4("projection", projection);
		default_shader->SetMat4("view", view);

		DrawEntity(test_entity_id);

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
			// do entity ui shit
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

		EntityData current_entity_data = entities_data.at(current_entity_id);
		
		ImGui::PushFont(font_body);
		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputText(" ", &current_entity_data.name);
		
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::PushItemWidth(200.0f);
			ImGui::DragFloat3("Position ", (float*)&current_entity_data.position, drag_step, -1000, 1000);
			ImGui::DragFloat3("Scale ", (float*)&current_entity_data.scale, drag_step, 0, 1000);
			ImGui::DragFloat3("Rotation ", (float*)&current_entity_data.rotation, drag_step, 0, 360);
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

	void Renderer::DrawModel(ParamReference ref)
	{
		
	}
	void Renderer::DrawEntity(EntityID id)
	{
		EntityData data = entities_data.at(id);
		DrawModel(data.params.at(PARAM_TYPE::MODEL));
	}
	glm::quat Renderer::GetTransformOrientation(EntityID id)
	{
		EntityData data = entities_data.at(id);
		return glm::quat(data.rotation / 180.0f * glm::pi<float>());
	}
	glm::mat4 Renderer::GetTransformMatrix(EntityID id)
	{
		EntityData data = entities_data.at(id);
		return glm::translate(glm::mat4(1.0f), data.position)
		* glm::toMat4(GetTransformOrientation())
		* glm::scale(glm::mat4(1.0f), data.scale);
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
