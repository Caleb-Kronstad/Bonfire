#include "bonfire_pch.hpp"
#include "Renderer.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{
    void Renderer::OnInterfaceUpdate()
	{
		Project& project = Project::GetInstance();
		Interface& project_interface = Project::GetInterface();
		Window& project_window = project.GetWindow();
		
		// -- VIEWPORT --
		ImGui::PushFont(font_title);
		ImGui::Begin("Viewport");
		DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
    	
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::SetWindowFocus();
			engine_camera_can_rotate = true;
			glfwSetInputMode(project_window.GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

    	DrawViewport();
    	
    	ImGui::End();

    	// -- DEBUG --
    	ImGui::PushFont(font_title);
    	ImGui::Begin("Debug", nullptr);
    	DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing(); 
    	ImGui::PopFont();
    	ImGui::PushFont(font_body);
    	
    	std::string delta_time = "Delta Time: " + std::to_string(project.GetDeltaTime());
    	std::string frame_time = "Frame Time: " + std::to_string(project.GetDeltaTime() * 1000.0f);
    	std::string frame_rate = "Frame Rate: " + std::to_string(1.0f / project.GetDeltaTime());
    	ImGui::Text(delta_time.c_str());
    	ImGui::Text(frame_time.c_str());
    	ImGui::Text(frame_rate.c_str());

    	const char* debug_type_names[] = { "DEFAULT", "WIREFRAME", "POINT" };
    	int current_debug_type = static_cast<int>(debug_type);
    	if (ImGui::Combo("Debug Mode", &current_debug_type, debug_type_names, IM_ARRAYSIZE(debug_type_names)))
    	{
    		debug_type = static_cast<DebugType>(current_debug_type);
    	}
    	
    	ImGui::PopFont();
    	ImGui::Unindent(8.0f);
    	ImGui::End();

		// -- PROJECT SETTINGS --
		ImGui::PushFont(font_title);
		ImGui::Begin("Project Settings", nullptr);
		DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
		ImGui::Indent(8.0f);
		ImGui::Spacing(); 
		ImGui::PopFont();
		ImGui::PushFont(font_body);

		DrawProjectSettings();
		
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();

		// -- CONSOLE --
		ImGui::PushFont(font_title);
		ImGui::Begin("Console", nullptr);
		DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);
		
		DrawConsole();
		
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();

    	// -- Toolbar --
    	ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    	
    	ImGui::PushFont(font_title);
    	ImGui::Begin("Toolbar", nullptr, toolbar_flags);
    	DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	ImGui::PopFont();
    	ImGui::PushFont(font_body);

    	DrawToolbar();

    	ImGui::PopFont();
    	ImGui::Unindent(8.0f);
    	ImGui::End();

		// -- HIERARCHY --
		ImGui::PushFont(font_title);
		ImGui::Begin("Hierarchy", nullptr);
		DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);
		ImGui::PushStyleColor(ImGuiCol_Header, project_interface.background_primary);

    	DrawHierarchy();
		
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();

    	if (entity_to_create != nullptr)
    	{
			CreateEntity(entity_to_create);
    		entity_to_create = nullptr;
    	}
    	if (entity_to_delete != nullptr)
    	{
    		DeleteEntity(entity_to_delete);
    		entity_to_delete = nullptr;
    	}
    	if (entity_to_reparent != nullptr)
    	{
    		ReparentEntity(entity_to_reparent, reparent_target);
    		entity_to_reparent = nullptr;
    		reparent_target = nullptr;
    	}

		// -- DETAILS --
		ImGui::PushFont(font_title);
		ImGui::Begin("Details", nullptr);
		DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);
		
		DrawDetails();
    	
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();

		// -- PARAM EDITOR --
		ImGui::PushFont(font_title);
		ImGui::Begin("Param Editor", nullptr);
		DrawActiveTitleLine(project_interface.highlight_primary, project_interface.background_tertiary);
		ImGui::Indent(8.0f);
		ImGui::Spacing();
		ImGui::PopFont();
		ImGui::PushFont(font_body);

		DrawParamEditor();
		
		ImGui::PopFont();
		ImGui::Unindent(8.0f);
		ImGui::End();
	}

	void Renderer::DrawViewport()
    {
    	viewport_focused = ImGui::IsWindowFocused();
		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		
		if (!FloatEquals(viewport_panel_size.x, viewport_size.x) || !FloatEquals(viewport_panel_size.y, viewport_size.y))
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
    			selected_entity->UpdateComponents(Project::GetPhysicsSystem());
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
    }

	void Renderer::DrawToolbar()
    {
    	Project& project = Project::GetInstance();
    	PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Interface& project_interface = Project::GetInterface();
    	Window& project_window = project.GetWindow();
    	
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_E) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::TRANSLATE;
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_R) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::ROTATE;
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_T) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::SCALE;

    	bool project_running = project.GetProjectRunState();
    	if (project_running)
    		ImGui::PushStyleColor(ImGuiCol_Button, project_interface.highlight_primary);
    	if (ImGui::ImageButton((void*)play_icon->gl_id, ImVec2(20, 20)))
    	{
    		// play
    		if (!project.GetProjectRunState())
    		{
    			Log::Info("Running...");
    			scene->SaveScene(*param_database);
    			project.SetProjectRunState(true);
    			physics_system.paused = false;
    			selected_entity = nullptr;
    		}
    		// stop playing
    		else if (project.GetProjectRunState())
    		{
    			Log::Info("Stopping...");
    			physics_system.paused = true;
    			project.SetProjectRunState(false);
    			scene->LoadScene(*param_database);
    		}
    	}
    	if (project_running)
    		ImGui::PopStyleColor(1);

    	ImGui::SameLine();

    	int temp_gizmo_type = gizmo_type;
    	if (temp_gizmo_type == ImGuizmo::TRANSLATE)
    		ImGui::PushStyleColor(ImGuiCol_Button, project_interface.highlight_primary);
    	if (ImGui::ImageButton((void*)move_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::TRANSLATE;
    	if (temp_gizmo_type == ImGuizmo::TRANSLATE)
    		ImGui::PopStyleColor(1);
		
    	ImGui::SameLine();
    	if (temp_gizmo_type == ImGuizmo::ROTATE)
    		ImGui::PushStyleColor(ImGuiCol_Button, project_interface.highlight_primary);
    	if (ImGui::ImageButton((void*)rotate_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::ROTATE;
    	if (temp_gizmo_type == ImGuizmo::ROTATE)
    		ImGui::PopStyleColor(1);
    	
    	ImGui::SameLine();
    	if (temp_gizmo_type == ImGuizmo::SCALE)
    		ImGui::PushStyleColor(ImGuiCol_Button, project_interface.highlight_primary);
    	if (ImGui::ImageButton((void*)resize_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::SCALE;
    	if (temp_gizmo_type == ImGuizmo::SCALE)
    		ImGui::PopStyleColor(1);
    }

	void Renderer::DrawProjectSettings()
    {
		Project& project = Project::GetInstance();
		
    	ImGui::PushItemWidth(100.0f);
    	ImGui::DragFloat("DragStep", &drag_step, 0.1f, 0.0f, 100.0f);
    	ImGui::PopItemWidth();

    	ImGui::Separator();
    	ImGui::PushItemWidth(300.0f);
    	ImGui::Text("Directional Light");
    	ImGui::SliderFloat3("Direction", (float*)&scene->GetDirectionalLight()->direction, -360.0f, 360.0f);
    	ImGui::SliderFloat3("Color", (float*)&scene->GetDirectionalLight()->color, 0.0f, 255.0f);
    	ImGui::PopItemWidth();
    }

	void Renderer::DrawConsole()
    {
    	std::vector<std::string> lines = console_capture->GetLines();
    	for (const std::string& line : lines)
    	{
    		auto [color, text] = ParseAnsiLine(line);
    		ImGui::PushTextWrapPos(0.0f);
    		ImGui::TextColored(color, "%s", text.c_str());
    		ImGui::PopTextWrapPos();
    	}
    }

	void Renderer::DrawHierarchy()
    {
		Interface& project_interface = Project::GetInterface();
    	
    	if (ImGui::BeginPopupContextWindow())
    	{
    		if (ImGui::MenuItem("Create Entity"))
    		{
    			CreateEntity();
    			ImGui::CloseCurrentPopup();
    		}
    		ImGui::EndPopup();
    	}
		
    	for (auto& [entity_id, entity] : scene->GetEntities())
    	{
    		if (entity->IsRoot())
    			DrawEntityTree(entity);
    	}

    	ImGui::Spacing();

    	float available_width = ImGui::GetContentRegionAvail().x;
    	float available_height = ImGui::GetContentRegionAvail().y;
    	float button_width = available_width * ((available_width - 25.0f) / available_width);
    	float button_height = available_height * ((available_width - 5.0f) / available_width);
    	float indent = (available_width - button_width) * 0.5f;
    	
    	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);
    	ImGui::PushStyleColor(ImGuiCol_Button, project_interface.background_primary);
    	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, project_interface.background_primary);
    	ImGui::PushStyleColor(ImGuiCol_ButtonActive, project_interface.background_primary);
    	ImGui::Button("##MoveButton", ImVec2(button_width, button_height));

    	if (ImGui::BeginDragDropTarget())
    	{
    		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE"))
    		{
    			uint32_t dragged_entity_id = *(const uint32_t*)payload->Data;
    			if (scene->GetEntities().contains(dragged_entity_id))
    			{
    				std::shared_ptr<Entity> dragged_entity = scene->GetEntities().at(dragged_entity_id);
    				entity_to_reparent = dragged_entity;
    				reparent_target = nullptr;
    			}
    		}
    		ImGui::EndDragDropTarget();
    	}
    	ImGui::PopStyleColor(3);
    }

	void Renderer::DrawDetails()
    {
    	PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    	
	    if (selected_entity == nullptr)
    	{
    		ImGui::Text("No Entity Selected");
    	}
    	else
    	{
    		ImGui::Checkbox("##Enabled", &selected_entity->enabled);
    		ImGui::SameLine();
    		ImGui::SetNextItemWidth(200.0f);
    		ImGui::InputText(" ", &selected_entity->name);
    		ImGui::SameLine();
    		ImGui::Text(std::to_string(selected_entity->id).c_str());
		
    		ImGui::Separator();
    		ImGui::Text("Transform");
    		ImGui::Spacing();
		
    		ImGui::PushItemWidth(200.0f);
    		if (ImGui::DragFloat3("Position ", (float*)&selected_entity->position, drag_step, -1000, 1000))
    			selected_entity->UpdateComponents(physics_system);
    		if (ImGui::DragFloat3("Scale ", (float*)&selected_entity->scale, drag_step, 0, 1000))
    			selected_entity->UpdateComponents(physics_system);
    		if (ImGui::DragFloat3("Rotation ", (float*)&selected_entity->rotation, drag_step, 0, 360))
    			selected_entity->UpdateComponents(physics_system);
    		ImGui::PopItemWidth();

			// MODEL COMPONENT
    		if (selected_entity->HasComponent<ModelComponent>())
			{
				ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();
    			
				ImGui::Separator();
    			ImGui::Checkbox("##Enabled", &model_component.enabled);
    			ImGui::SameLine();
				ImGui::Text("Model Component");
    			ImGui::SameLine();
    			ImGui::Text(std::to_string(model_component.id).c_str());
				ImGui::Spacing();

    			ImGui::Checkbox("Casts Shadow", &model_component.model->casts_shadow);

    			if (ImGui::Button(model_component.model->name.c_str(), ImVec2(100, 22)))
    				ImGui::OpenPopup("ChangeModelModelComponent");
    			ImGui::SameLine(); ImGui::Text("Model");
    			if (ImGui::Button(model_component.shader->name.c_str(), ImVec2(100, 22)))
    				ImGui::OpenPopup("ChangeShaderModelComponent");
    			ImGui::SameLine(); ImGui::Text("Shader");
    			if (ImGui::Button(model_component.material->name.c_str(), ImVec2(100, 22)))
    				ImGui::OpenPopup("ChangeMaterialModelComponent");
    			ImGui::SameLine(); ImGui::Text("Material");

    			if (ImGui::BeginPopup("ChangeModelModelComponent"))
    			{
    				for (auto& [id, scene_item] : scene->GetModels())
    				{
    					ImGui::PushID(&id);
    					if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    					{
    						model_component.model = scene_item;
    						ImGui::CloseCurrentPopup();
    					}
    					ImGui::PopID();
    				}
    				ImGui::EndPopup();
    			}
    			if (ImGui::BeginPopup("ChangeShaderModelComponent"))
    			{
    				for (auto& [id, scene_item] : scene->GetShaders())
    				{
    					ImGui::PushID(&id);
    					if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    					{
    						model_component.shader = scene_item;
    						ImGui::CloseCurrentPopup();
    					}
    					ImGui::PopID();
    				}
    				ImGui::EndPopup();
    			}
    			if (ImGui::BeginPopup("ChangeMaterialModelComponent"))
    			{
    				for (auto& [id, scene_item] : scene->GetMaterials())
    				{
    					ImGui::PushID(&id);
    					if (ImGui::Selectable(scene_item->name.c_str(), false, 0))
    					{
    						model_component.material = scene_item;
    						ImGui::CloseCurrentPopup();
    					}
    					ImGui::PopID();
    				}
    				ImGui::EndPopup();
    			}
			}

    		if (selected_entity->HasComponent<LightSourceComponent>())
    		{
				LightSourceComponent& light_source_component = selected_entity->GetComponent<LightSourceComponent>();
    			
    			ImGui::Separator();

    			ImGui::Separator();
    			ImGui::Checkbox("##Enabled", &light_source_component.enabled);
    			ImGui::SameLine();
    			ImGui::Text("Light Source Component");
    			ImGui::SameLine();
    			ImGui::Text(std::to_string(light_source_component.id).c_str());
    			ImGui::Spacing();
    			
    			if (ImGui::Button(light_source_component.light_source->name.c_str(), ImVec2(100, 22)))
    				ImGui::OpenPopup("ChangeLightSourceComponentLightSource");
    			ImGui::SameLine(); ImGui::Text("Light Source Type");

    			if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
    			{
    				ImGui::Spacing();
    				ImGui::Text("Point Light Properties");
    				ImGui::PushItemWidth(200.0f);
    				ImGui::SliderFloat("Intensity", &point_light->intensity, 0.0f, 10.0f);
    				ImGui::SliderFloat3("Color", (float*)&point_light->color, 0.0f, 255.0f);
    				ImGui::PopItemWidth();
    			}
    			else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
    			{
    				ImGui::Spacing();
    				ImGui::Text("Spot Light Properties");
    				ImGui::PushItemWidth(200.0f);
    				ImGui::SliderFloat3("Color", (float*)&spot_light->color, 0.0f, 255.0f);
    				ImGui::DragFloat3("Scale", (float*)&spot_light->scale, drag_step, 0.0f, 100.0f);
    				ImGui::DragFloat3("Direction", (float*)&spot_light->direction, drag_step, -1.0f, 1.0f);
    				ImGui::PopItemWidth();
    			}

    			if (ImGui::BeginPopup("ChangeLightSourceComponentLightSource"))
    			{
    				auto old_light = light_source_component.light_source;
					glm::vec3 preserved_color = glm::vec3(255.0f);
					glm::vec3 preserved_position = glm::vec3(0.0f);
					glm::vec3 preserved_scale = glm::vec3(1.0f);
					glm::vec3 preserved_direction = glm::vec3(0.0f);
    				float preserved_intensity = 1.0f;

					if (auto point_light = std::dynamic_pointer_cast<PointLight>(old_light))
					{
						preserved_color = point_light->color;
						preserved_position = point_light->position;
						preserved_scale = point_light->scale;
						preserved_intensity = point_light->intensity;
					}
					else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(old_light))
					{
					    preserved_color = spot_light->color;
					    preserved_position = spot_light->position;
					    preserved_scale = spot_light->scale;
					    preserved_direction = spot_light->direction;
					}
    				
					if (ImGui::MenuItem("Point Light"))
					{
						if (auto current_type = std::dynamic_pointer_cast<PointLight>(old_light))
							ImGui::CloseCurrentPopup();
						if (auto old_spot = std::dynamic_pointer_cast<SpotLight>(old_light))
							scene->GetSpotLights().erase(old_spot->id);

						std::shared_ptr<PointLight> new_light = std::make_shared<PointLight>();
						new_light->color = preserved_color;
						new_light->position = preserved_position;
						new_light->scale = preserved_scale;
						new_light->intensity = preserved_intensity;
						new_light->id = old_light->id;
						new_light->enabled = old_light->enabled;
						light_source_component.light_source = new_light;

						scene->GetPointLights().insert_or_assign(new_light->id, new_light);
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::MenuItem("Spot Light"))
					{
						if (auto current_type = std::dynamic_pointer_cast<SpotLight>(old_light))
							ImGui::CloseCurrentPopup();
						if (auto old_point = std::dynamic_pointer_cast<PointLight>(old_light))
							scene->GetPointLights().erase(old_point->id);

						std::shared_ptr<SpotLight> new_light = std::make_shared<SpotLight>();
						new_light->color = preserved_color;
						new_light->position = preserved_position;
						new_light->scale = preserved_scale;
						new_light->direction = preserved_direction;
						new_light->id = old_light->id;
						new_light->enabled = old_light->enabled;
						light_source_component.light_source = new_light;

						scene->GetSpotLights().insert_or_assign(new_light->id, new_light);
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
    			}
    		}
    		
			// PHYSICS COMPONENT
			if (selected_entity->HasComponent<PhysicsComponent>())
			{
				PhysicsComponent& physics_component = selected_entity->GetComponent<PhysicsComponent>();
				
				ImGui::Separator();
				
				ImGui::Separator();
				ImGui::Checkbox("##Enabled", &physics_component.enabled);
				ImGui::SameLine();
				ImGui::Text("Physics Component");
				ImGui::SameLine();
				ImGui::Text(std::to_string(physics_component.id).c_str());
				ImGui::Spacing();
				
				int selected_body_type = static_cast<int>(physics_component.physics_body->GetBodyType());
				int selected_shape_type = static_cast<int>(physics_component.physics_body->GetShapeData().type);
				PhysicsShapeData current_shape_data = physics_component.physics_body->GetShapeData();

				const char* body_type_names[] = { "STATIC", "DYNAMIC", "KINEMATIC" };
				const char* shape_type_names[] = { "BOX", "SPHERE", "CAPSULE" };

				bool body_type_changed = ImGui::Combo("Body Type", &selected_body_type, body_type_names,
				IM_ARRAYSIZE(body_type_names));
				bool shape_type_changed = ImGui::Combo("Shape Type", &selected_shape_type, shape_type_names,
				IM_ARRAYSIZE(shape_type_names));

				if (body_type_changed || shape_type_changed)
				{
					PhysicsBodyType new_body_type = static_cast<PhysicsBodyType>(selected_body_type);
					PhysicsShapeType new_shape_type = static_cast<PhysicsShapeType>(selected_shape_type);

					glm::vec3 position = physics_component.physics_body->GetPosition();
					glm::quat rotation = physics_component.physics_body->GetRotation();
					uint32_t id = physics_component.physics_body->id;
					bool enabled = physics_component.physics_body->enabled;
					std::string name = physics_component.physics_body->name;
					glm::vec3 dimensions = current_shape_data.dimensions;

					std::shared_ptr<PhysicsBody> new_physics_body;

					if (new_shape_type == PhysicsShapeType::BOX)
						new_physics_body = physics_system.CreateBoxBody(position, rotation, dimensions, new_body_type);
					else if (new_shape_type == PhysicsShapeType::SPHERE)
						new_physics_body = physics_system.CreateSphereBody(position, dimensions.x, new_body_type);
					else if (new_shape_type == PhysicsShapeType::CAPSULE)
						new_physics_body = physics_system.CreateCapsuleBody(position, rotation, dimensions.x, dimensions.y, new_body_type);

					new_physics_body->id = id;
					new_physics_body->enabled = enabled;
					new_physics_body->name = name;
					new_physics_body->SetEnabled(enabled);

					physics_component.physics_body = new_physics_body;
				}

				selected_entity->UpdateComponents(physics_system);
			}

			// ANIMATION COMPONENT
			if (selected_entity->HasComponent<PhysicsComponent>())
			{
				ImGui::Separator();
				
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
						uint32_t next_id = 100001;
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
							std::shared_ptr<Material> default_material = scene->GetMaterials().begin()->second;
							std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(next_id, true, default_model, default_shader, default_material);

							scene->GetModelComponents().insert_or_assign(next_id, new_component);
							selected_entity->AddComponent(ComponentType::MODEL, new_component);
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

    			if (ImGui::MenuItem("Light Source Component"))
    			{
    				if (!selected_entity->HasComponent<LightSourceComponent>())
    				{
    					uint32_t next_id = 100001;
    					if (!scene->GetLightSourceComponents().empty())
    					{
    						auto max_it = std::max_element(
								scene->GetLightSourceComponents().begin(),
								scene->GetLightSourceComponents().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
							);
    						next_id = max_it->first + 1;
    					}

    					uint32_t next_light_id = 1000;
    					if (!scene->GetPointLights().empty())
    					{
    						auto max_it = std::max_element(
								scene->GetPointLights().begin(),
								scene->GetPointLights().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
							);
    						next_light_id = max_it->first + 1;
    					}
    					
    					std::shared_ptr<PointLight> new_light = std::make_shared<PointLight>();
    					new_light->id = next_light_id;
    					new_light->position = selected_entity->position;
    					new_light->color = glm::vec3(255.0f, 255.0f, 255.0f);
    					new_light->scale = glm::vec3(1.0f);
    					new_light->intensity = 1.0f;
    					new_light->enabled = true;

    					std::shared_ptr<LightSourceComponent> new_component = std::make_shared<LightSourceComponent>(next_id, true, new_light);

    					scene->GetLightSourceComponents().insert_or_assign(next_id, new_component);
    					scene->GetPointLights().insert_or_assign(next_light_id, new_light);
    					selected_entity->AddComponent(ComponentType::LIGHT, new_component);
    				}
				    else
				    {
					    Log::Warning("Entity already has light source component");
				    	ImGui::CloseCurrentPopup();
				    }
    			}
    			
    			if (ImGui::MenuItem("Physics Component"))
    			{
    				if (!selected_entity->HasComponent<PhysicsComponent>())
    				{
    					uint32_t next_id = 100001;
    					if (!scene->GetPhysicsComponents().empty())
    					{
    						auto max_it = std::max_element(
								scene->GetPhysicsComponents().begin(),
								scene->GetPhysicsComponents().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
							);
    						next_id = max_it->first + 1;
    					}
    					
    					uint32_t next_po_id = 1000;
    					if (!scene->GetPhysicsComponents().empty())
    					{
    						auto max_it = std::max_element(
								scene->GetPhysicsComponents().begin(),
								scene->GetPhysicsComponents().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
							);
    						next_po_id = max_it->first + 1;
    					}

    					std::string physics_name = "Physics Object";
    					PhysicsBodyType body_type = PhysicsBodyType::DYNAMIC;
    					PhysicsShapeType shape_type = PhysicsShapeType::BOX;
    					glm::vec3 dimensions = selected_entity->scale / 2.0f;

    					std::shared_ptr<PhysicsBody> physics_body = physics_system.CreateBoxBody(selected_entity->position, glm::quat(glm::radians(selected_entity->rotation)), dimensions, body_type);
    					physics_body->id = next_po_id;
    					physics_body->enabled = true;
    					physics_body->SetEnabled(true);
    					physics_body->name = physics_name;

    					std::shared_ptr<PhysicsComponent> physics_component = std::make_shared<PhysicsComponent>(next_id, true, physics_body);
    					scene->GetPhysicsComponents().insert_or_assign(next_id, physics_component);
    					selected_entity->AddComponent(ComponentType::PHYSICS, physics_component);
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
		
    }

	void Renderer::DrawParamEditor()
    {
		Interface& project_interface = Project::GetInterface();
    	
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
				std::filesystem::path models_dir = exe_dir / "Data/Resources/Models";
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
					ofn.lpstrFilter = "Model Files\0*.obj;*.fbx;*.dae;*.gltf;*.glb\0Obj Files\0*.obj\0FBX Files\0*.fbx\0DAE Files\0*.dae\0glTF Files\0*.gltf;*.glb\0All Files\0*.*\0";
					ofn.lpstrTitle = "Select model file";

					if (GetOpenFileNameA(&ofn))
					{
						model_file.resize(model_file.find('\0'));

						std::filesystem::path absolute_path = model_file;
						std::string abs_str = absolute_path.string();

						size_t data_pos = abs_str.find("Data");
						if (data_pos != std::string::npos)
						{
							new_model_path = abs_str.substr(data_pos);
							std::replace(new_model_path.begin(), new_model_path.end(), '\\', '/');
						}
						else
							new_model_path = model_file;

						Log::Info("File selected at " + new_model_path);
					
						uint32_t next_id = 1000;
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

						std::vector<std::shared_ptr<Texture>> auto_textures;
						for (const auto& [tex_path, tex_type] : new_model->extracted_texture_paths)
						{
							std::shared_ptr<Texture> existing_texture = nullptr;
							for (const auto& [id, tex] : scene->GetTextures())
							{
								if (tex->path == tex_path)
								{
									existing_texture = tex;
									break;
								}
							}

							if (existing_texture)
							{
								auto_textures.push_back(existing_texture);
							}
							else
							{
								uint32_t tex_id = 1;
								if (!scene->GetTextures().empty())
								{
									auto max_it = std::max_element(
										scene->GetTextures().begin(),
										scene->GetTextures().end(),
										[](const auto& a, const auto& b) { return a.first < b.first; }
									);
									tex_id = max_it->first + 1;
								}

								std::filesystem::path tex_path_obj(tex_path);
								std::string tex_name = tex_path_obj.stem().string();

								std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(tex_path, tex_type, false);
								new_texture->param_id = tex_id;
								new_texture->name = tex_name;
								new_texture->Load();

								scene->GetTextures().insert_or_assign(tex_id, new_texture);
								param_database->texture_params[tex_id] = TextureParamData(tex_name, tex_type, false, tex_path);

								auto_textures.push_back(new_texture);
								Log::Info("Loaded texture: " + tex_name);
							}
						}
						if (!auto_textures.empty())
						{
							uint32_t mat_id = 1000;
							if (!scene->GetMaterials().empty())
							{
								auto max_it = std::max_element(
									scene->GetMaterials().begin(),
									scene->GetMaterials().end(),
									[](const auto& a, const auto& b) { return a.first < b.first; }
								);
								mat_id = max_it->first + 1;
							}

							std::string mat_name = model_name + "_material";
							std::shared_ptr<Material> new_material = std::make_shared<Material>(mat_name);
							new_material->param_id = mat_id;

							for (auto& tex : auto_textures)
							{
								new_material->AddTexture(tex);
							}

							scene->GetMaterials().insert_or_assign(mat_id, new_material);

							std::vector<uint32_t> texture_ids;
							for (auto& tex : auto_textures)
							{
								texture_ids.push_back(tex->param_id);
							}
							param_database->material_params[mat_id] = MaterialParamData(mat_name, texture_ids);

							Log::Info("Created material: " + mat_name + " with " + std::to_string(auto_textures.size()) + " texture(s)");
						}
						
						scene->GetModels().insert_or_assign(next_id, new_model);
						param_database->model_params.insert_or_assign(next_id, ModelParamData(model_name, new_model_path));
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
		                            texture_data.type = static_cast<TextureType>(n);
		                        }
		                        
		                        if (is_selected)
		                            ImGui::SetItemDefaultFocus();
		                    }
		                    ImGui::EndCombo();
		                }

		            	GLuint texture_gl_id = scene->GetTextures().at(texture_id)->gl_id;
		            	ImGui::Image((void*)texture_gl_id, ImVec2(100,100));
		            }
		        	ImGui::PopID();
		        }

		    	ImGui::Separator();

		    	char exe_path[MAX_PATH];
		    	GetModuleFileNameA(NULL, exe_path, MAX_PATH);
		    	std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
		    	std::filesystem::path textures_dir = exe_dir / "Data/Resources/Textures";
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

		    			size_t data_pos = abs_str.find("Data");
		    			if (data_pos != std::string::npos)
		    			{
		    				new_texture_path = abs_str.substr(data_pos);
		    				std::replace(new_texture_path.begin(), new_texture_path.end(), '\\', '/');
		    			}
		    			else
		    				new_texture_path = texture_file;

		    			Log::Info("File selected at " + new_texture_path);
					
		    			uint32_t next_id = 200001;
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
					
		    			std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(new_texture_path, TextureType::DIFFUSE, false);
		    			new_texture->param_id = next_id;
		    			new_texture->name = texture_name;
		    			new_texture->Load();
		    			scene->GetTextures().insert_or_assign(next_id, new_texture);
		    			param_database->texture_params[next_id] = TextureParamData(texture_name, TextureType::DIFFUSE, false, new_texture_path);
		    		}
		    		else
		    			Log::Info("File operation cancelled");
		    	}
		    	
		        ImGui::EndTabItem();
		    }

			if (ImGui::BeginTabItem("Material Params"))
			{
				for (auto& [material_id, material_data] : scene->GetMaterials())
				{
					ImGui::PushID(&material_id);
					if (ImGui::CollapsingHeader(std::to_string(material_id).c_str()))
					{
						ImGui::SetNextItemWidth(200.0f);
						ImGui::InputText("Name", &material_data->name);

						ImGui::Text("Textures");

						std::shared_ptr<Texture> texture_to_remove = nullptr;
						
						for (auto& texture_data : material_data->textures)
						{
							ImGui::PushID(&texture_data);
							ImGui::Text("%u", texture_data->param_id);
							ImGui::Image((void*)texture_data->gl_id, ImVec2(100,100));
							if (ImGui::Button("-"))
							{
								if (material_data->textures.size() > 1)
									texture_to_remove = texture_data;
								else
									Log::Warning("Material must have at least 1 texture");
							}
							ImGui::PopID();
						}
						if (texture_to_remove != nullptr)
							material_data->RemoveTexture(texture_to_remove);

						if (ImGui::Button("+"))
						{
							ImGui::OpenPopup("AddTextureMaterialPopup");
						}
						if (ImGui::BeginPopup("AddTextureMaterialPopup"))
						{
							for (auto& [texture_id, texture_data] : scene->GetTextures())
							{
								ImGui::PushID(&texture_id);
								ImGui::Image((void*)(intptr_t)texture_data->gl_id, ImVec2(25, 25));
								ImGui::SameLine(); 
								if (ImGui::Selectable(texture_data->name.c_str(), false, 0, ImVec2(150, 25)))
								{
									scene->GetMaterials().at(material_id)->AddTexture(scene->GetTextures().at(texture_id));
									ImGui::CloseCurrentPopup();
								}

								ImGui::PopID();
							}
							ImGui::EndPopup();
						}
					}
					ImGui::PopID();
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
    }
	
	void Renderer::DrawEntityTree(std::shared_ptr<Entity> entity)
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

    	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    	{
    		ImGui::SetDragDropPayload("ENTITY_NODE", &entity->id, sizeof(uint32_t));
    		ImGui::EndDragDropSource();
    	}

    	if (ImGui::BeginDragDropTarget())
    	{
    		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE"))
    		{
    			uint32_t dragged_entity_id = *(const uint32_t*)payload->Data;
    			if (scene->GetEntities().contains(dragged_entity_id))
    			{
    				std::shared_ptr<Entity> dragged_entity = scene->GetEntities().at(dragged_entity_id);
    				if (dragged_entity->id != entity->id && !IsDescendentOf(dragged_entity, entity))
    				{
    					entity_to_reparent = dragged_entity;
    					reparent_target = entity;
    				}
    			}
    		}
    		ImGui::EndDragDropTarget();
    	}

    	if (ImGui::BeginPopupContextItem("EntityPopupWindow"))
    	{
    		if (ImGui::MenuItem("Create"))
    		{
    			entity_to_create = entity;
    			ImGui::CloseCurrentPopup();
    		}
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
    				DrawEntityTree(scene->GetEntities()[child_id]);
    		}
    		ImGui::TreePop();
    	}
    	ImGui::PopID();
    }

}