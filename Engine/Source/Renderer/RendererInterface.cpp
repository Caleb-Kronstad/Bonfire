#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{
    void Renderer::OnInterfaceUpdate()
	{
		Project& project = Project::GetInstance();
		Interface& project_interface = project.GetInterface();
		Window& project_window = project.GetWindow();
		ImVec4& highlight_color = project_interface.highlight_primary;
    	ImVec4& inactive_color = project_interface.background_tertiary;
		
		// -- VIEWPORT --
		ImGui::PushFont(font_title);
		ImGui::Begin("Viewport");
		DrawActiveTitleLine(highlight_color, inactive_color);
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
    	ImVec2 viewport_min = ImGui::GetItemRectMin();
    	ImVec2 viewport_max = ImGui::GetItemRectMax();
    	float viewport_width = viewport_max.x - viewport_min.x;
    	float viewport_height = viewport_max.y - viewport_min.y;

    	if (selected_entity != nullptr && gizmo_type != -1)
    	{
    		ImGuizmo::SetOrthographic(scene->GetEngineCamera()->isOrthographic);
    		ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    		ImGuizmo::SetRect(viewport_min.x, viewport_min.y, viewport_width, viewport_height);
    		const glm::mat4& camera_view = scene->GetEngineCamera()->GetViewMatrix();
    		const glm::mat4& camera_projection = scene->GetEngineCamera()->GetProjectionMatrix(viewport_width, viewport_height);
    		glm::mat4 transform = selected_entity->GetWorldTransformMatrix(scene->GetEntities());

    		ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection), (ImGuizmo::OPERATION)gizmo_type, (ImGuizmo::MODE)gizmo_space, glm::value_ptr(transform));

    		if (ImGuizmo::IsUsing())
    		{
    			glm::mat4 local_transform = transform;

    			if (!selected_entity->IsRoot() && scene->GetEntities().contains(selected_entity->parent))
    			{
    				glm::mat4 parent_world_transform = scene->GetEntities().at(selected_entity->parent)->GetWorldTransformMatrix(scene->GetEntities());
    				local_transform = glm::inverse(parent_world_transform) * transform;
    			}
    			glm::vec3 translation, rotation, scale;
    			DecomposeTransform(local_transform, translation, rotation, scale);
    			selected_entity->position = translation;
    			selected_entity->rotation = glm::degrees(rotation);
    			selected_entity->scale = scale;
    		}
    	}

    	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver())
    	{
    		std::shared_ptr<Entity> hit_entity = nullptr;
    		float closest_distance = FLT_MAX;

    		ImVec2 mouse_pos = ImGui::GetMousePos();

    		const glm::mat4& camera_view = scene->GetEngineCamera()->GetViewMatrix();
    		const glm::mat4& camera_projection = scene->GetEngineCamera()->GetProjectionMatrix(viewport_width, viewport_height);

    		Ray ray = ScreenPointToRay(
    			glm::vec2(mouse_pos.x, mouse_pos.y),
    			glm::vec2(viewport_min.x, viewport_min.y),
    			glm::vec2(viewport_width, viewport_height),
    			camera_view,
    			camera_projection
    			);

    		for (auto& [entity_id, entity] : scene->GetEntities())
    		{
    			if (!entity->enabled)
    				continue;

    			AABB world_aabb = entity->GetWorldAABB(scene->GetEntities());
    			if (world_aabb.minimum.x > world_aabb.maximum.x)
    				continue;

    			float t;
    			if (RayIntersectsAABB(ray, world_aabb, t))
    			{
    				if (t < closest_distance)
    				{
    					closest_distance = t;
    					hit_entity = entity;
    				}
    			}
    		}

    		if (hit_entity != nullptr)
    			selected_entity = hit_entity;
    		else
    			selected_entity = nullptr;
    	}
    	
    	ImGui::End();

		// -- PROJECT SETTINGS --
		ImGui::PushFont(font_title);
		ImGui::Begin("Project Settings", nullptr);
		DrawActiveTitleLine(highlight_color, inactive_color);
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
		DrawActiveTitleLine(highlight_color, inactive_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);
		
		std::vector<std::string> lines = console_capture->GetLines();
		for (const std::string& line : lines)
		{
			auto [color, text] = ParseAnsiLine(line);
			ImGui::PushTextWrapPos(0.0f);
			ImGui::TextColored(color, "%s", text.c_str());
    		ImGui::PopTextWrapPos();
		}
		
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();

    	// -- Toolbar --
    	ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    	
    	ImGui::PushFont(font_title);
    	ImGui::Begin("Toolbar", nullptr, toolbar_flags);
    	DrawActiveTitleLine(highlight_color, inactive_color);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	ImGui::PopFont();
    	ImGui::PushFont(font_body);

    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_E) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::TRANSLATE;
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_R) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::ROTATE;
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_T) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::SCALE;

    	if (ImGui::ImageButton((void*)move_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::TRANSLATE;
    	ImGui::SameLine();
    	if (ImGui::ImageButton((void*)rotate_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::ROTATE;
    	ImGui::SameLine();
    	if (ImGui::ImageButton((void*)resize_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::SCALE;

    	ImGui::PopFont();
    	ImGui::Unindent(8.0f);
    	ImGui::End();

		// -- HIERARCHY --
		ImGui::PushFont(font_title);
		ImGui::Begin("Hierarchy", nullptr);
		DrawActiveTitleLine(highlight_color, inactive_color);
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

    	if (entity_to_delete != nullptr)
    	{
    		DeleteEntity(entity_to_delete);
    		entity_to_delete = nullptr;
    	}

		// -- DETAILS --
		ImGui::PushFont(font_title);
		ImGui::Begin("Details", nullptr);
		DrawActiveTitleLine(highlight_color, inactive_color);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);
		
    	if (selected_entity == nullptr)
    	{
    		ImGui::Text("No Entity Selected");
    	}
    	else
    	{
    		ImGui::SetNextItemWidth(200.0f);
    		ImGui::InputText(" ", &selected_entity->name);
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(selected_entity->id).c_str());
		
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
						ImGui::PushID(&scene_model);
						
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

				preview_value = model_component.shader->name.c_str();
				if (ImGui::BeginCombo("Shader", preview_value))
				{
					for (auto& [shader_id, scene_shader] : scene->GetShaders())
					{
						ImGui::PushID(&scene_shader);
						
						bool is_selected = (model_component.shader->param_id == shader_id);

						if (ImGui::Selectable(scene_shader->name.c_str(), is_selected))
						{
							model_component.shader = scene_shader;
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
					ImGui::PushID(i);

					const char* preview_value = texture->name.c_str();
					if (ImGui::BeginCombo("##", preview_value))
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
		
			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponentPopup");

    		if (ImGui::BeginPopup("AddComponentPopup"))
    		{
				if (ImGui::MenuItem("Model Component"))
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
							std::shared_ptr<Shader> default_shader = scene->GetShaders().begin()->second;
							std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(next_id, true, default_model, default_shader);

							scene->GetModelComponents().insert_or_assign(next_id, new_component);
							selected_entity->AddComponent(COMPONENT_TYPE::MODEL, new_component);
						}
						else
							Log::Warning("No models available");
					}
					else
					{
						Log::Warning("Entity already has model component");
						ImGui::CloseCurrentPopup();
					}
				}

    			if (ImGui::MenuItem("Texture Component"))
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
    				{
						Log::Warning("Entity already has texture component");
    					ImGui::CloseCurrentPopup();
    				}
    			}

    			if (ImGui::MenuItem("Physics Component"))
    			{
    				if (!selected_entity->HasComponent<PhysicsComponent>())
    				{
						Log::Warning("Physics component not yet implemented");
    				}
    				else
    				{
    					Log::Warning("Entity already has physics component");
    					ImGui::CloseCurrentPopup();
    				}
    			}

    			if (ImGui::MenuItem("Animation Component"))
    			{
    				if (!selected_entity->HasComponent<AnimationComponent>())
    				{
						Log::Warning("Animation component not yet implemented");
    				}
    				else
    				{
    					Log::Warning("Entity already has animation component");
    					ImGui::CloseCurrentPopup();
    				}
    			}
    			ImGui::EndPopup();
    		}
    	
			ImGui::PopID();
    	}
		
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();

		// -- PARAM EDITOR --
		ImGui::PushFont(font_title);
		ImGui::Begin("Param Editor", nullptr);
		DrawActiveTitleLine(highlight_color, inactive_color);
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
					
						std::shared_ptr<Model> new_model = std::make_shared<Model>(new_model_path);
						new_model->param_id = next_id;
						new_model->name = model_name;
						new_model->Load();
						scene->GetModels().insert_or_assign(next_id, new_model);
						param_database->model_params[next_id] = ModelParamData(model_name, new_model_path);
					}
					else
						Log::Info("File operation cancelled");
				}
				
		        ImGui::EndTabItem();
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

		    	ImGui::Separator();

		    	char exe_path[MAX_PATH];
		    	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
		    	std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
		    	std::filesystem::path textures_dir = exe_dir / "Assets/Resources/Textures";
		    	std::string texture_file = std::string(MAX_PATH, '\0');
		    	
		    	if (ImGui::Button("+"))
		    	{
		    		OPENFILENAMEA ofn;
		    		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		    		ofn.lStructSize = sizeof(OPENFILENAME);
		    		ofn.lpstrFile = (LPSTR)texture_file.c_str();
		    		ofn.nMaxFile = texture_file.size();
		    		ofn.lpstrInitialDir = textures_dir.string().c_str();
		    		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
		    		ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.gif;*.tif;*.tiff;*.dds;*.hdr\0PNG Files\0*.png\0JPEG Files\0*.jpg;*.jpeg\0BMP Files\0*.bmp\0TGA Files\0*.tga\0All Files\0*.*\0";
		    		ofn.lpstrTitle = "Select texture file";

		    		if (GetOpenFileNameA(&ofn))
		    		{
		    			texture_file.resize(texture_file.find('\0'));

		    			std::filesystem::path absolute_path = texture_file;
		    			std::string abs_str = absolute_path.string();

		    			size_t assets_pos = abs_str.find("Assets");
		    			if (assets_pos != std::string::npos)
		    			{
		    				new_texture_path = abs_str.substr(assets_pos);
		    				std::replace(new_texture_path.begin(), new_texture_path.end(), '\\', '/');
		    			}
		    			else
		    				new_texture_path = texture_file;

		    			Log::Info("File selected at " + new_texture_path);
					
		    			uint32_t next_id = 1;
		    			if (!scene->GetTextures().empty())
		    			{
		    				auto max_it = std::max_element(
								scene->GetTextures().begin(),
								scene->GetTextures().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
								);
		    				next_id = max_it->first + 1;
		    			}

		    			std::filesystem::path path_obj(new_texture_path);
		    			std::string texture_name = path_obj.stem().string();
					
		    			std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(new_texture_path, TEXTURE_TYPE::DIFFUSE, false);
		    			new_texture->param_id = next_id;
		    			new_texture->name = texture_name;
		    			new_texture->Load();
		    			scene->GetTextures().insert_or_assign(next_id, new_texture);
		    			param_database->texture_params[next_id] = TextureParamData(texture_name, TEXTURE_TYPE::DIFFUSE, false, new_texture_path);
		    		}
		    		else
		    			Log::Info("File operation cancelled");
		    	}
		    	
		        ImGui::EndTabItem();
		    }

			if (ImGui::BeginTabItem("Shader Params"))
			{
				for (auto& [shader_id, shader_data] : param_database->shader_params)
				{
					ImGui::PushID(&shader_data);
					if (ImGui::CollapsingHeader(std::to_string(shader_id).c_str()))
					{
						ImGui::Text(("Vertex Shader: " + shader_data.vert_path).c_str());
						ImGui::Text(("Fragment Shader: " + shader_data.frag_path).c_str());
						ImGui::Text(("Geometry Shader: " + shader_data.geom_path).c_str());
						ImGui::SetNextItemWidth(200.0f);
						ImGui::InputText("Name", &shader_data.name);
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

	void Renderer::RenderEntityTree(std::shared_ptr<Entity> entity)
    {
    	ImGui::PushID(&entity->id);
    	
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

    	if (ImGui::BeginPopupContextItem())
    	{
    		if (ImGui::MenuItem("Duplicate"))
    		{
    			DuplicateEntity(entity);
    			ImGui::CloseCurrentPopup();
    		}
    		if (ImGui::MenuItem("Delete"))
    		{
    			entity_to_delete = entity;
    			ImGui::CloseCurrentPopup();
    		}
    		ImGui::EndPopup();	
    	}

    	if (node_open && !entity->children.empty())
    	{
    		for (uint32_t child_id : entity->children)
    		{
    			if (scene->GetEntities().contains(child_id))
    				RenderEntityTree(scene->GetEntities()[child_id]);
    		}
    		ImGui::TreePop();
    	}
    	ImGui::PopID();
    }

}