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
		engine_camera_can_rotate = false;

		param_database = std::make_unique<ParamDatabase>("Assets/Params/models.params", "Assets/Params/textures.params");
		new_model_path = "Assets/Resources/Models/Cube.obj";
		new_texture_path = "Assets/Resources/Textures/Checkered.png";

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
		default_shader = Shader("Default", "Assets/Resources/Shaders/default.vert", "Assets/Resources/Shaders/default.frag", "None");
		default_shader.Use();
		default_shader.SetVec4("color", glm::vec4(0.3f, 0.8f, 0.7f, 1.0f));
		// -------
		
		// SCENE AND EDITOR LOADING
		scene = std::make_unique<Scene>("Assets/Scenes/testscene.bonfirescene");
		Load();
		if (!scene->GetEntities().empty())
			selected_entity = scene->GetEntities().begin()->second;
		
		
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
				scene->GetEngineCamera()->ProcessKeyboard(MOVEMENT_DIRECTION::FORWARD, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::S) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MOVEMENT_DIRECTION::BACKWARD, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::A) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MOVEMENT_DIRECTION::LEFT, deltaTime);
			if (glfwGetKey(glfw_window, InputCode::D) == GLFW_PRESS)
				scene->GetEngineCamera()->ProcessKeyboard(MOVEMENT_DIRECTION::	RIGHT, deltaTime);
		}
		// ---

		viewport_framebuffer->Bind();
		
		glClearColor(background_color.r, background_color.g, background_color.b, background_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = scene->GetEngineCamera()->GetProjectionMatrix(viewport_size.x, viewport_size.y);
		glm::mat4 view = scene->GetEngineCamera()->GetViewMatrix();

		default_shader.Use();
		default_shader.SetMat4("projection", projection);
		default_shader.SetMat4("view", view);

		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			entity->Draw(default_shader, manipulation_matrix, scene->GetEntities());
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

		std::string delta_time = "Delta Time: " + std::to_string(project.GetDeltaTime());
		ImGui::Text(delta_time.c_str());
		
		ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("DragStep", &drag_step, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();
		
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

		// -- HIERARCHY --
		ImGui::PushFont(font_title);
		ImGui::Begin("Hierarchy", nullptr);
		DrawActiveTitleLine(highlight_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);
		ImGui::PushStyleColor(ImGuiCol_Header, project_interface.background_primary);
		
		for (auto& [entity_id, entity] : scene->GetEntities())
		{
			if (entity->IsRoot())
				RenderEntityTree(entity);
		}

		if (ImGui::Button("+"))
		{
			uint32_t next_id = 1000001;
			if (!scene->GetEntities().empty())
			{
				auto max_it = std::max_element(
					scene->GetEntities().begin(),
					scene->GetEntities().end(),
					[](const auto& a, const auto& b) { return a.first < b.first; }
					);
				next_id = max_it->first+1;
			}

			std::shared_ptr<Entity> new_entity = std::make_shared<Entity>(next_id, true, "New Entity");
			scene->GetEntities().insert_or_assign(next_id, new_entity);
			selected_entity = new_entity;
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
		
		ImGui::SetNextItemWidth(-1.0f);
		ImGui::InputText(" ", &selected_entity->name);
		
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::Spacing();
		
		ImGui::PushItemWidth(200.0f);
		ImGui::DragFloat3("Position ", (float*)&selected_entity->position, drag_step, -1000, 1000);
		ImGui::DragFloat3("Scale ", (float*)&selected_entity->scale, drag_step, 0, 1000);
		ImGui::DragFloat3("Rotation ", (float*)&selected_entity->rotation, drag_step, 0, 360);
		ImGui::PopItemWidth();

		// MODEL COMPONENT
		if (selected_entity->HasComponent<ModelComponent>())
		{
			ImGui::Separator();
			ImGui::Text("Model Component");
			ImGui::Spacing();
			
			ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();

			const char* preview_value = model_component.model->name.c_str();
			if (ImGui::BeginCombo("Model", preview_value))
			{
				for (auto& [model_id, scene_model] : scene->GetModels())
				{
					ImGui::PushID(model_id);
					
					bool is_selected = (model_component.model->param_id == model_id);

					if (ImGui::Selectable(scene_model->name.c_str(), is_selected))
					{
						model_component.model = scene_model;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();

					ImGui::PopID();
				}
				ImGui::EndCombo();
			}

			ImGui::Spacing();
		}

		// TEXTURE COMPONENT
		if (selected_entity->HasComponent<TextureComponent>())
		{
			ImGui::Separator();
			ImGui::Text("Texture Component");
			ImGui::Spacing();
			
			TextureComponent& texture_component = selected_entity->GetComponent<TextureComponent>();
			for (int i = texture_component.textures.size() - 1; i >= 0; i--)
			{
				auto& texture = texture_component.textures[i];
				ImGui::PushID(i);  // Use index as ID

				const char* preview_value = texture->name.c_str();
				if (ImGui::BeginCombo(" ", preview_value))
				{
					for (auto& [texture_id, scene_texture] : scene->GetTextures())
					{
						bool is_selected = (texture == scene_texture);

						if (ImGui::Selectable(scene_texture->name.c_str(), is_selected))
						{
							texture = scene_texture;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::SameLine();

				if (ImGui::Button("-"))
				{
					texture_component.RemoveTexture(texture);
				}

				ImGui::PopID();
			}

			ImGui::Spacing();

			if (ImGui::Button("+"))
			{
				texture_component.AddTexture(scene->GetTextures().begin()->second);
			}
			
			ImGui::Spacing();
		}

		// PHYSICS COMPONENT
		if (selected_entity->HasComponent<PhysicsComponent>())
		{
			
		}

		// ANIMATION COMPONENT
		if (selected_entity->HasComponent<PhysicsComponent>())
		{
			
		}

		ImGui::Separator();

		// ADD COMPONENT
		ImGui::PushID("##NEWCOMPONENT");
		ImGui::PushStyleColor(ImGuiCol_Button, project_interface.background_secondary);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, project_interface.highlight_secondary);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, project_interface.highlight_primary);
		if (ImGui::BeginCombo(" ", "Add Component"))
		{
			if (ImGui::Button("Model Component"))
			{
				if (!selected_entity->HasComponent<ModelComponent>())
				{
					uint32_t next_id = 1;
					if (!scene->GetModelComponents().empty())
					{
						auto max_it = std::max_element(
							scene->GetModelComponents().begin(),
							scene->GetModelComponents().end(),
							[](const auto& a, const auto& b) { return a.first < b.first; }
							);
						next_id = max_it->first + 1;
					}

					if (!scene->GetModels().empty())
					{
						std::shared_ptr<Model> default_model = scene->GetModels().begin()->second;
						std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(next_id, true, default_model);

						scene->GetModelComponents().insert_or_assign(next_id, new_component);
						selected_entity->AddComponent(COMPONENT_TYPE::MODEL, new_component);
					}
					else
						Log::Warning("No models available");
				}
				else
					Log::Warning("Entity already has a Model Component");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Texture Component"))
			{
				if (!selected_entity->HasComponent<TextureComponent>())
				{
					uint32_t next_id = 1;
					if (!scene->GetTextureComponents().empty())
					{
						auto max_it = std::max_element(
							scene->GetTextureComponents().begin(),
							scene->GetTextureComponents().end(),
							[](const auto& a, const auto& b) { return a.first < b.first; }
							);
						next_id = max_it->first + 1;
					}

					std::shared_ptr<TextureComponent> new_component = std::make_shared<TextureComponent>(next_id, true, std::vector<std::shared_ptr<Texture>>());

					scene->GetTextureComponents().insert_or_assign(next_id, new_component);
					selected_entity->AddComponent(COMPONENT_TYPE::TEXTURE, new_component);
				}
				else
					Log::Warning("Entity already has a Texture Component");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Physics Component"))
			{
				Log::Warning("Physics component not yet implemented");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Animation Component"))
			{
				Log::Warning("Physics component not yet implemented");
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndCombo();
		}
		ImGui::PopStyleColor(3);
		ImGui::PopID();
		
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
		        for (auto& [model_id, model_data] : param_database->model_params)
		        {
		        	ImGui::PushID(&model_data);
		            if (ImGui::CollapsingHeader(std::to_string(model_id).c_str()))
		            {
		            	ImGui::Text(model_data.path.c_str());
		                ImGui::SetNextItemWidth(200.0f);
		            	ImGui::InputText("Name", &model_data.name);
		            }
		        	ImGui::PopID();
		        }
		        ImGui::EndTabItem();

				ImGui::Separator();

				char exe_path[MAX_PATH];
				GetModuleFileNameA(NULL, exe_path, MAX_PATH);
				std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
				std::filesystem::path models_dir = exe_dir / "Assets/Resources/Models";
				std::string model_file = std::string(MAX_PATH, '\0');

				if (ImGui::Button("+"))
				{
					OPENFILENAMEA ofn;
					ZeroMemory(&ofn, sizeof(OPENFILENAME));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrFile = (LPSTR)model_file.c_str();
					ofn.nMaxFile = model_file.size();
					ofn.lpstrInitialDir = models_dir.string().c_str();
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
					ofn.lpstrFilter = "Model Files\0*.obj;*.fbx;*.dae\0Obj Files\0*.obj\0FBX Files\0*.fbx\0DAE Files\0*.dae\0All Files\0*.*\0";
					ofn.lpstrTitle = "Select model file";

					if (GetOpenFileNameA(&ofn))
					{
						model_file.resize(model_file.find('\0'));

						std::filesystem::path absolute_path = model_file;
						std::string abs_str = absolute_path.string();

						size_t assets_pos = abs_str.find("Assets");
						if (assets_pos != std::string::npos)
						{
							new_model_path = abs_str.substr(assets_pos);
							std::replace(new_model_path.begin(), new_model_path.end(), '\\', '/');
						}
						else
							new_model_path = model_file;

						Log::Info("File selected at " + new_model_path);
					}
					else
						Log::Warning("File operation cancelled");
					
					uint32_t next_id = 1;
					if (!scene->GetModels().empty())
					{
						auto max_it = std::max_element(
							scene->GetModels().begin(),
							scene->GetModels().end(),
							[](const auto& a, const auto& b) { return a.first < b.first; }
							);
						next_id = max_it->first + 1;
					}

					std::filesystem::path path_obj(new_model_path);
					std::string model_name = path_obj.stem().string();

					Log::Info("Current working dir: " + std::filesystem::current_path().string());
					Log::Info("Trying to load: " + new_model_path);
					Log::Info("New path: [" + new_model_path + "]");
					Log::Info("Existing path: [" + param_database->model_params[1000].path + "]");
					
					std::shared_ptr<Model> new_model = std::make_shared<Model>(new_model_path);
					new_model->param_id = next_id;
					new_model->name = model_name;
					new_model->Load();
					scene->GetModels().insert_or_assign(next_id, new_model);
					param_database->model_params[next_id] = ModelParamData(model_name, new_model_path);
				}
		    }
		    
		    if (ImGui::BeginTabItem("Texture Params"))
		    {
		        const char* texture_type_names[] = { "Diffuse", "Specular", "Normal", "Height" };
		        for (auto& [texture_id, texture_data] : param_database->texture_params)
		        {
		        	ImGui::PushID(&texture_data);
		            if (ImGui::CollapsingHeader(std::to_string(texture_id).c_str()))
		            {
		                ImGui::Text(texture_data.path.c_str());
		                ImGui::SetNextItemWidth(200.0f);
		                ImGui::InputText("Name", &texture_data.name);
		                
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
		            }
		        	ImGui::PopID();
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

				if (scene->GetEngineCamera()->firstMouse)
				{
					scene->GetEngineCamera()->lastX = x_position;
					scene->GetEngineCamera()->lastY = y_position;
					scene->GetEngineCamera()->firstMouse = false;
				}

				const float x_offset = x_position - scene->GetEngineCamera()->lastX;
				const float y_offset = y_position - scene->GetEngineCamera()->lastY;

				scene->GetEngineCamera()->lastX = x_position;
				scene->GetEngineCamera()->lastY = y_position;

				if (engine_camera_can_rotate)
					scene->GetEngineCamera()->ProcessMouseMovement(x_offset, y_offset);
				
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

	void Renderer::RenderEntityTree(std::shared_ptr<Entity> entity)
	{
		Project& project = Project::GetInstance();
		Interface& project_interface = project.GetInterface();
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		bool is_selected = (entity == selected_entity);

		if (is_selected)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (entity->children.empty())
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (is_selected)
		{
			ImGui::PushStyleColor(ImGuiCol_Header, project_interface.highlight_primary);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, project_interface.highlight_secondary);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, project_interface.highlight_primary);
		}

		bool node_open = ImGui::TreeNodeEx(entity->name.c_str(), flags);

		if (is_selected)
			ImGui::PopStyleColor(3);

		if (ImGui::IsItemClicked())
			selected_entity = entity;

		if (node_open && !entity->children.empty())
		{
			for (uint32_t child_id : entity->children)
			{
				if (scene->GetEntities().contains(child_id))
					RenderEntityTree(scene->GetEntities()[child_id]);
			}
			ImGui::TreePop();
		}
	}

	bool Renderer::Load()
	{
		param_database->LoadParams();
		bool scene_loaded = scene->LoadScene(*param_database);
		return scene_loaded;
	}
	bool Renderer::Save()
	{
		bool scene_saved = scene->SaveScene(*param_database);
		param_database->SaveParams();
		return scene_saved;
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
