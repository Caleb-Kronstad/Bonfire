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
		Project& project = Project::GetInstance();
		Interface& project_interface = project.GetInterface();
		Window& project_window = project.GetWindow();
		
		console_capture = std::make_unique<ConsoleCapture>();
		console_capture->StartCapture();
		//console_capture->StopCapture(); // uncomment if editor console is not running properly
		
		manipulation_matrix = glm::mat4(1.0f);
		engine_camera = std::make_unique<Camera>();
		engine_camera_can_rotate = false;

		param_database = std::make_unique<ParamDatabase>("Assets/Params/modelparams.bonfireparams", "Assets/Params/textureparams.bonfireparams");

		background_color = RgbToGlmVec4(22, 22, 22, 1.0f);
		viewport_framebuffer = std::make_unique<Framebuffer>(viewport_size.x, viewport_size.y);
		
		std::stringstream path_stream;
		path_stream << "Project Path: " << std::filesystem::current_path();
		Log::Info(path_stream.str());

		// LOAD FONTS
		ImGuiIO& io = ImGui::GetIO();
		font_title = io.Fonts->AddFontFromFileTTF("Assets/Resources/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		font_body = io.Fonts->AddFontFromFileTTF("Assets/Resources/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());

		// --- TESTING ---
		default_shader = std::make_unique<Shader>("Default", "Assets/Resources/Shaders/default.vert", "Assets/Resources/Shaders/default.frag", "None");
		default_shader->Use();
		default_shader->SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
		// -------
		
		// SCENE AND EDITOR LOADING
		scene = std::make_unique<Scene>("Assets/Scenes/testscene.bonfirescene");
		Load();
		if (!entities.empty())
			current_entity_id = entities[0];
		
	}
	void Renderer::OnDetach()
	{
		console_capture->StopCapture();
	}

	void Renderer::OnUpdate()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		GLFWwindow* glfw_window = project_window.GetNativeWindow();
		const float deltaTime = project.GetDeltaTime();

		// --- TESTING - IMPROVE IMPLEMENTATION AT LATER TIME ---
		if (project_window.GetWidth() <= 0 || project_window.GetHeight() <= 0)
			return;
		if (viewport_focused)
		{
			if (glfwGetKey(glfw_window, InputCode::W) == GLFW_PRESS)
				engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::FORWARD, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::S) == GLFW_PRESS)
				engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::BACKWARD, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::A) == GLFW_PRESS)
				engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::LEFT, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::D) == GLFW_PRESS)
				engine_camera->ProcessKeyboard(MOVEMENT_DIRECTION::	RIGHT, deltaTime);
		}
		// ---

		viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = engine_camera->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		glm::mat4 view = engine_camera->GetViewMatrix();

		default_shader->Use();
		default_shader->SetMat4("projection", projection);
		default_shader->SetMat4("view", view);

		for (auto entity_id : entities)
		{
			DrawEntity(entity_id);
		}

		viewport_framebuffer->Unbind();
		glViewport(0, 0, project_window.GetWidth(), project_window.GetHeight());

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
		ImGui::PushFont(font_body);

		std::string frame_count = "Frame " + std::to_string(project.GetFrameCount());
		std::string delta_time = "Delta Time: " + std::to_string(project.GetDeltaTime());
		
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
		
		for (auto entity_id : entities)
		{
			EntityData entity_data = entities_data.at(entity_id);
			if (ImGui::Selectable(entity_data.name.c_str()))
			{
				current_entity_id = entity_id;
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
		ImGui::PushFont(font_body);

		EntityData& current_entity_data = entities_data.at(current_entity_id);
		
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

		// -- PARAM EDITOR --
		ImGui::PushFont(font_title);
		ImGui::Begin("Param Editor", nullptr);
		DrawActiveTitleLine(highlight_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);

		if (ImGui::BeginTabBar("ParamEditorTabs"))
		{
			ImGui::PushStyleColor(ImGuiCol_TabActive, project_interface.highlight_primary);
			ImGui::PushStyleColor(ImGuiCol_TabHovered, project_interface.highlight_secondary);
			ImGui::PushStyleColor(ImGuiCol_Tab, project_interface.background_tertiary);
			ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, project_interface.highlight_primary);
			if (ImGui::BeginTabItem("Model Params"))
		    {
		        for (auto& [model_ref, model_data] : param_database->model_params)
		        {
		            if (ImGui::CollapsingHeader(model_data.name.c_str()))
		            {
		                ImGui::PushID(&model_data);
		                
		                ImGui::SetNextItemWidth(-1.0f);
		                ImGui::InputText(" ", &model_data.name);
		                ImGui::Text(model_data.path.c_str());
		                ImGui::DragFloat3("Rotation Multiplier ", (float*)&model_data.rotation_multiplier, drag_step, 0, 100);
		                ImGui::DragFloat3("Scale Multiplier", (float*)&model_data.scale_multiplier, drag_step, 0, 100);
		                
		                ImGui::PopID();
		            }
		        }
		        ImGui::EndTabItem();
		    }
		    
		    if (ImGui::BeginTabItem("Texture Params"))
		    {
		        const char* texture_type_names[] = { "Diffuse", "Specular", "Normal", "Height" };
		        for (auto& [texture_ref, texture_data] : param_database->texture_params)
		        {
		            if (ImGui::CollapsingHeader(texture_data.name.c_str()))
		            {
		                ImGui::PushID(&texture_data);
		                
		                ImGui::SetNextItemWidth(-1.0f);
		                ImGui::InputText(" ", &texture_data.name);
		                
		                ImGui::Text(texture_data.path.c_str());
		                
		                ImGui::Checkbox("Flip", &texture_data.flip);
		                
		                int current_index = static_cast<int>(texture_data.type);
		                if (ImGui::BeginCombo("Type", texture_type_names[current_index]))
		                {
		                    for (int n = 0; n < IM_ARRAYSIZE(texture_type_names); n++)
		                    {
		                        bool is_selected = (current_index == n);
		                        if (ImGui::Selectable(texture_type_names[n], is_selected))
		                        {
		                            texture_data.type = static_cast<TEXTURE_TYPE>(n);
		                        }
		                        
		                        if (is_selected)
		                            ImGui::SetItemDefaultFocus();
		                    }
		                    ImGui::EndCombo();
		                }
		                
		                ImGui::PopID();
		            }
		        }
		        ImGui::EndTabItem();
		    }
			ImGui::EndTabBar();
			ImGui::PopStyleColor(4);
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

	void Renderer::DrawModel(ParamReference model_ref, ParamReference texture_ref)
	{
		Model& model = models.at(model_ref);
		Texture& texture = textures.at(texture_ref);
		std::vector<std::shared_ptr<Texture>> textures;
		textures.push_back(std::make_shared<Texture>(texture));
		model.Draw(*default_shader, textures);
	}
	void Renderer::DrawEntity(EntityID id)
	{
		EntityData data = entities_data.at(id);
		if (!data.enabled) return;
		manipulation_matrix = GetTransformMatrix(id);
		default_shader->SetMat4("model", manipulation_matrix);
		DrawModel(data.params.at(PARAM_TYPE::MODEL), data.params.at(PARAM_TYPE::TEXTURE));
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
		* glm::toMat4(GetTransformOrientation(id))
		* glm::scale(glm::mat4(1.0f), data.scale);
	}

	bool Renderer::Load()
	{
		bool loaded = scene->LoadScene();
		entities = scene->GetEntities();
		entities_data = scene->GetEntitiesData();
		engine_camera = std::move(scene->GetSceneCamera());
		
		param_database->LoadParams();

		for (const auto& entity_id : entities)
		{
			EntityData& entity_data = entities_data.at(entity_id);

			if (entity_data.params.contains(PARAM_TYPE::MODEL))
			{
				ParamReference model_ref = entity_data.params.at(PARAM_TYPE::MODEL);
				if (!models.contains(model_ref))
				{
					ModelParamData model_param = param_database->GetModelParam(model_ref);
					Model model(model_param.path);
					model.Load();
					models.insert_or_assign(model_ref, model);
				}
			}

			if (entity_data.params.contains(PARAM_TYPE::TEXTURE))
			{
				ParamReference texture_ref = entity_data.params.at(PARAM_TYPE::TEXTURE);
				if (!textures.contains(texture_ref))
				{
					TextureParamData texture_param = param_database->GetTextureParam(texture_ref);
					Texture texture(texture_param.path, texture_param.type, texture_param.flip);
					texture.Load();
					textures.insert_or_assign(texture_ref, texture);
				}
			}

			if (entity_data.params.contains(PARAM_TYPE::AI))
			{
				// load ai param
			}

			if (entity_data.params.contains(PARAM_TYPE::PHYSICS))
			{
				// load physics param
			}

			if (entity_data.params.contains(PARAM_TYPE::ANIMATION))
			{
				// load animation param
			}
		}

		return loaded;
	}
	bool Renderer::Save()
	{
		scene->GetEntities() = entities;
		scene->GetEntitiesData() = entities_data;
		scene->GetSceneCamera() = std::move(engine_camera);
		bool saved = scene->SaveScene();
		entities = scene->GetEntities();
		entities_data = scene->GetEntitiesData();
		engine_camera = std::move(scene->GetSceneCamera());
		
		param_database->SaveParams();
		
		return saved;
	}
	void Renderer::ReloadParams(ParamReference ref, PARAM_TYPE type)
	{
		if (type == PARAM_TYPE::MODEL)
		{
			ModelParamData model_param = param_database->GetModelParam(ref);
			Model model(model_param.path);
			model.Load();
			models.insert_or_assign(ref, model);
		}
		else if (type == PARAM_TYPE::TEXTURE)
		{
			TextureParamData texture_param = param_database->GetTextureParam(ref);
			Texture texture(texture_param.path, texture_param.type, texture_param.flip);
			texture.Load();
			textures.insert_or_assign(ref, texture);
		}
	}
	
	void Renderer::DrawActiveTitleLine(const ImVec4& color, float thickness)
	{
		if (!ImGui::IsWindowFocused())
			return;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();

		if (window->DockNode && window->DockNode->TabBar)
		{
			ImGuiTabBar* tab_bar = window->DockNode->TabBar;
			ImGuiTabItem* tab = ImGui::TabBarFindTabByID(tab_bar, window->TabId);

			if (tab)
			{
				float tab_x = tab_bar->BarRect.Min.x + tab->Offset;
				float tab_y = tab_bar->BarRect.Min.y;
				float tab_width = tab->Width;

				draw_list->AddRectFilled(
					ImVec2(tab_x, tab_y),
					ImVec2(tab_x + tab_width, tab_y + thickness),
					ImGui::ColorConvertFloat4ToU32(color)
				);
				return;
			}
		}

		ImVec2 window_pos = ImGui::GetWindowPos();
		const char* title = window->Name;
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
