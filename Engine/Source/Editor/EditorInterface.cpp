#include "bonfire_pch.hpp"
#include "Editor.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{
	void Editor::OnInterfaceUpdate()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);
    	
		DrawMenuBar();
		DrawEditorViewport();
		DrawProjectViewport();
		DrawToolbar();
		DrawDebugInfo();
		DrawConsole();
		DrawProjectSettings();
		DrawParamEditor();
		DrawHierarchy();
		DrawDetails();
	}

	void Editor::OnInterfaceEndUpdate()
	{
		ImGui::Render();
		ImGui::EndFrame();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Editor::UpdateInterfaceStyle()
	{
		text_primary = RgbaToImVec4(255, 255, 255);
		background_primary = RgbaToImVec4(38, 38, 38);
		background_secondary = RgbaToImVec4(23, 23, 23);
		background_tertiary = RgbaToImVec4(56, 56, 56);
		highlight_primary = RgbaToImVec4(116, 77, 169);
		highlight_secondary = RgbaToImVec4(141, 124, 192);
		SetInterfaceStyle();
	}

	void Editor::SetInterfaceStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		
		colors[ImGuiCol_Text] = text_primary;
		colors[ImGuiCol_WindowBg] = background_primary;
		colors[ImGuiCol_Border] = background_tertiary;
		colors[ImGuiCol_FrameBg] = background_secondary;
		colors[ImGuiCol_FrameBgHovered] = highlight_primary;
		colors[ImGuiCol_FrameBgActive] = highlight_secondary;
		colors[ImGuiCol_TitleBg] = background_secondary;
		colors[ImGuiCol_TitleBgActive] = background_secondary;
		colors[ImGuiCol_TitleBgCollapsed] = background_secondary;
		colors[ImGuiCol_SliderGrab] = highlight_primary;
		colors[ImGuiCol_SliderGrabActive] = highlight_primary;
		colors[ImGuiCol_Header] = background_tertiary;
		colors[ImGuiCol_HeaderHovered] = highlight_primary;
		colors[ImGuiCol_HeaderActive] = highlight_primary;
		colors[ImGuiCol_TextSelectedBg] = highlight_primary;
		colors[ImGuiCol_CheckMark] = highlight_primary;
		colors[ImGuiCol_Button] = background_tertiary;
		colors[ImGuiCol_ButtonHovered] = highlight_secondary;
		colors[ImGuiCol_ButtonActive] = highlight_primary;
		colors[ImGuiCol_Separator] = background_tertiary;
		colors[ImGuiCol_SeparatorHovered] = highlight_primary;
		colors[ImGuiCol_SeparatorActive] = highlight_primary;
		colors[ImGuiCol_ResizeGrip] = highlight_primary;
		colors[ImGuiCol_ResizeGripHovered] = highlight_primary;
		colors[ImGuiCol_ResizeGripActive] = highlight_primary;
		colors[ImGuiCol_Tab] = background_primary;
		colors[ImGuiCol_TabHovered] = background_tertiary;
		colors[ImGuiCol_TabActive] = background_primary;
		colors[ImGuiCol_TabUnfocused] = background_primary;
		colors[ImGuiCol_TabUnfocusedActive] = background_primary;
		colors[ImGuiCol_DockingPreview] = highlight_primary;
		colors[ImGuiCol_DockingEmptyBg] = background_secondary;
		colors[ImGuiCol_PlotHistogram] = highlight_primary;
		colors[ImGuiCol_PlotHistogramHovered] = highlight_secondary;
		colors[ImGuiCol_DragDropTarget] = highlight_secondary;
		
		style->WindowRounding = 0.0f;
		style->FrameRounding = 0.0f;
		style->GrabRounding = 0.0f;
		style->PopupRounding = 0.0f;
		style->TabRounding = 0.0f;
		style->WindowMenuButtonPosition = ImGuiDir_Right;
		style->ScrollbarSize = 10.0f;
		style->GrabMinSize = 10.0f;
		style->DockingSeparatorSize = 1.0f;
		style->SeparatorTextBorderSize = 2.0f;
		style->WindowPadding = ImVec2(0.0f, 0.0f);
	}
	
	void Editor::DrawMenuBar()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();

	    // Menu Bar
	    if (ImGui::BeginMenuBar())
	    {
	        if (ImGui::BeginMenu("File"))
	        {
	            if (ImGui::MenuItem("Reload", "Ctrl+L"))
	            {
	            	LoadEditorConfig();
		            renderer.Load();
	            }
	            if (ImGui::MenuItem("Save", "Ctrl+S"))
	            {
	            	SaveEditorConfig();
		            renderer.Save();
	            }
	            ImGui::Separator();
	            if (ImGui::MenuItem("Exit", "Alt+F4")) { project.SetEngineRunState(false); }
	            ImGui::EndMenu();
	        }
	        
	        if (ImGui::BeginMenu("Edit"))
	        {
	            if (ImGui::MenuItem("Undo", "Ctrl+Z")) { Log::Info("Undo"); }
	            if (ImGui::MenuItem("Redo", "Ctrl+Y")) { Log::Info("Redo"); }
	            ImGui::Separator();
	            if (ImGui::MenuItem("Cut", "Ctrl+X")) { Log::Info("Cut"); }
	            if (ImGui::MenuItem("Copy", "Ctrl+C")) { Log::Info("Copy"); }
	            if (ImGui::MenuItem("Paste", "Ctrl+V")) { Log::Info("Paste"); }
	            ImGui::EndMenu();
	        }
	        
	        if (ImGui::BeginMenu("Help"))
	        {
	            if (ImGui::MenuItem("Documentation")) { Log::Info("Documentation"); }
	            if (ImGui::MenuItem("About")) { Log::Info("Find more information at https://bonfireengine.com"); }
	            ImGui::EndMenu();
	        }
	        
	        ImGui::EndMenuBar();
	    }

	    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	    ImGui::End();
	}
	
	void Editor::DrawEditorViewport()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		editor_viewport_visible = ImGui::Begin("Viewport");
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
		
		if (!editor_viewport_visible)
		{
			ImGui::End();
			return;
		}
    	
    	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    	{
    		ImGui::SetWindowFocus();
    		engine_camera_can_turn = true;
    	}
    	
    	editor_viewport_focused = ImGui::IsWindowFocused();
		editor_viewport_hovered = ImGui::IsWindowHovered();
		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		
		if (!FloatEquals(viewport_panel_size.x, renderer.GetEditorViewportSize().x) || !FloatEquals(viewport_panel_size.y, renderer.GetEditorViewportSize().y))
		{
			if (viewport_panel_size.x > 0 && viewport_panel_size.y > 0)
			{
				renderer.GetEditorViewportSize() = {viewport_panel_size.x, viewport_panel_size.y};
				renderer.GetEditorViewportFramebuffer().Resize(renderer.GetEditorViewportSize().x, renderer.GetEditorViewportSize().y);
			}
		}

		ImGui::Image((void*)(intptr_t)renderer.GetEditorViewportFramebuffer().GetColorAttachment(), viewport_panel_size, ImVec2(0,1), ImVec2(1, 0));
    	ImVec2 viewport_min = ImGui::GetItemRectMin();
    	ImVec2 viewport_max = ImGui::GetItemRectMax();
    	float viewport_width = viewport_max.x - viewport_min.x;
    	float viewport_height = viewport_max.y - viewport_min.y;

    	if (selected_entity != nullptr && gizmo_type != -1)
    	{
    		ImGuizmo::SetOrthographic(engine_camera->IsOrthographic());
    		ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    		ImGuizmo::SetRect(viewport_min.x, viewport_min.y, viewport_width, viewport_height);
    		const glm::mat4& camera_view = engine_camera->GetViewMatrix();
    		const glm::mat4& camera_projection = engine_camera->GetProjectionMatrix(viewport_width, viewport_height);
    		glm::mat4 transform = selected_entity->GetWorldTransformMatrix(scene.GetEntities());

    		ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection), (ImGuizmo::OPERATION)gizmo_type, (ImGuizmo::MODE)gizmo_space, glm::value_ptr(transform));

    		if (ImGuizmo::IsUsing())
    		{
    			glm::mat4 local_transform = transform;

    			if (!selected_entity->IsRoot() && scene.GetEntities().contains(selected_entity->parent))
    			{
    				glm::mat4 parent_world_transform = scene.GetEntities().at(selected_entity->parent)->GetWorldTransformMatrix(scene.GetEntities());
    				local_transform = glm::inverse(parent_world_transform) * transform;
    			}
    			glm::vec3 translation, rotation, scale;
    			DecomposeTransform(local_transform, translation, rotation, scale);
    			selected_entity->position = translation;
    			selected_entity->rotation = glm::degrees(rotation);
    			selected_entity->scale.x = (std::max)(scale.x, 0.01f);
    			selected_entity->scale.y = (std::max)(scale.y, 0.01f);
    			selected_entity->scale.z = (std::max)(scale.z, 0.01f);
    			selected_entity->UpdateComponents(Project::GetPhysicsSystem());
    		}
    	}

    	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver())
    	{
    		std::shared_ptr<Entity> hit_entity = nullptr;
    		float closest_distance = FLT_MAX;

    		ImVec2 mouse_pos = ImGui::GetMousePos();

    		const glm::mat4& camera_view = engine_camera->GetViewMatrix();
    		const glm::mat4& camera_projection = engine_camera->GetProjectionMatrix(viewport_width, viewport_height);

    		Ray ray = ScreenPointToRay(
    			glm::vec2(mouse_pos.x, mouse_pos.y),
    			glm::vec2(viewport_min.x, viewport_min.y),
    			glm::vec2(viewport_width, viewport_height),
    			camera_view,
    			camera_projection
    			);

    		for (auto& [entity_id, entity] : scene.GetEntities())
    		{
    			if (!entity->enabled)
    				continue;

    			AABB world_aabb = entity->GetWorldAABB(scene.GetEntities());
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
    }

	void Editor::DrawProjectViewport()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		
		project_viewport_visible = ImGui::Begin("Project Name Here");
		DrawActiveTitleLine(highlight_primary, background_tertiary);
    	
		project_viewport_focused = ImGui::IsWindowFocused();
		project_viewport_hovered = ImGui::IsWindowHovered();
		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		
		if (!FloatEquals(viewport_panel_size.x, renderer.GetProjectViewportSize().x) || !FloatEquals(viewport_panel_size.y, renderer.GetProjectViewportSize().y))
		{
			if (viewport_panel_size.x > 0 && viewport_panel_size.y > 0)
			{
				renderer.GetProjectViewportSize() = {viewport_panel_size.x, viewport_panel_size.y};
				renderer.GetProjectViewportFramebuffer().Resize(renderer.GetProjectViewportSize().x, renderer.GetProjectViewportSize().y);
			}
		}

		

		ImGui::Image((void*)(intptr_t)renderer.GetProjectViewportFramebuffer().GetColorAttachment(), viewport_panel_size, ImVec2(0,1), ImVec2(1, 0));
		ImVec2 viewport_min = ImGui::GetItemRectMin();
		ImVec2 viewport_max = ImGui::GetItemRectMax();
		float viewport_width = viewport_max.x - viewport_min.x;
		float viewport_height = viewport_max.y - viewport_min.y;

		ImGui::End();
	}

	void Editor::DrawDebugInfo()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
    	
    	ImGui::Begin("Debug Info", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	
    	std::string delta_time = "Delta Time: " + std::to_string(project.GetDeltaTime());
    	std::string frame_time = "Frame Time: " + std::to_string(project.GetDeltaTime() * 1000.0f);
    	std::string frame_rate = "Frame Rate: " + std::to_string(std::lround((1.0f / project.GetDeltaTime())));
    	ImGui::Text(delta_time.c_str());
    	ImGui::Text(frame_time.c_str());
    	ImGui::Text(frame_rate.c_str());

		ImGui::Checkbox("Preview Animations", &preview_animations);

		ImGui::Checkbox("Draw Colliders", &renderer.GetDrawColliders());
		ImGui::SliderFloat("Collider Line Width", &renderer.GetDrawCollidersLineWidth(), 0.1f, 10.0f, "%.1f");

    	const char* debug_type_names[] = { "DEFAULT", "WIREFRAME", "POINT" };
    	int current_debug_type = static_cast<int>(renderer.GetDebugType());
    	if (ImGui::Combo("Debug Mode", &current_debug_type, debug_type_names, IM_ARRAYSIZE(debug_type_names)))
    	{
    		renderer.GetDebugType() = static_cast<DebugType>(current_debug_type);
    	}
    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
	}

	void Editor::DrawToolbar()
    {
		Project& project = Project::GetInstance();
    	PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		ParamDatabase& param_database = renderer.GetParamDatabase();
		
    	ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
    	
    	ImGui::Begin("Toolbar", nullptr, toolbar_flags);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_E) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::TRANSLATE;
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_R) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::ROTATE;
    	if (glfwGetKey(project_window.GetNativeWindow(), GLFW_KEY_T) == GLFW_PRESS)
    		gizmo_type = ImGuizmo::SCALE;

    	bool project_running = project.GetProjectRunState();
    	if (project_running)
    		ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    	if (ImGui::ImageButton((void*)play_icon->gl_id, ImVec2(20, 20)))
    	{
    		// play
    		if (!project.GetProjectRunState())
    		{
    			Log::Info("Running...");
    			serialized_scene_data = scene.SerializeToString(param_database);
    			project.SetProjectRunState(true);
    			selected_entity = nullptr;
    			ImGui::SetWindowFocus("Project Name Here");
    		}
    		// stop playing
    		else if (project.GetProjectRunState())
    		{
    			Log::Info("Stopping...");
    			project.SetProjectRunState(false);
    			scene.DeserializeFromString(serialized_scene_data, param_database);
    			selected_entity = nullptr;
    			ImGui::SetWindowFocus("Viewport");
    		}
    	}
    	if (project_running)
    		ImGui::PopStyleColor(1);

    	ImGui::SameLine();

    	int temp_gizmo_type = gizmo_type;
    	if (temp_gizmo_type == ImGuizmo::TRANSLATE)
    		ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    	if (ImGui::ImageButton((void*)move_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::TRANSLATE;
    	if (temp_gizmo_type == ImGuizmo::TRANSLATE)
    		ImGui::PopStyleColor(1);
		
    	ImGui::SameLine();
    	if (temp_gizmo_type == ImGuizmo::ROTATE)
    		ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    	if (ImGui::ImageButton((void*)rotate_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::ROTATE;
    	if (temp_gizmo_type == ImGuizmo::ROTATE)
    		ImGui::PopStyleColor(1);
    	
    	ImGui::SameLine();
    	if (temp_gizmo_type == ImGuizmo::SCALE)
    		ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    	if (ImGui::ImageButton((void*)resize_icon->gl_id, ImVec2(20, 20)))
    		gizmo_type = ImGuizmo::SCALE;
    	if (temp_gizmo_type == ImGuizmo::SCALE)
    		ImGui::PopStyleColor(1);

    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawProjectSettings()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
    	
    	ImGui::Begin("Project Settings", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing(); 
    	
    	

		ImGui::PushItemWidth(100.0f);
    	ImGui::DragFloat("Drag Step", &drag_step, 0.1f, 0.1f, 100.0f, "%.2f");

		ImGui::Separator();
		ImGui::Text("Engine Camera");
		ImGui::SliderFloat("Camera Speed", &engine_camera_speed, 0.01f, 100.0f, "%.1f");
		ImGui::SliderFloat("Camera Sensitivity", &engine_camera_turn_sensitivity, 0.01f, 10.0f, "%.1f");
    	ImGui::PopItemWidth();

    	ImGui::Separator();
    	ImGui::PushItemWidth(300.0f);
    	ImGui::Text("Directional Light");
    	ImGui::SliderFloat3("Direction", (float*)&scene.GetDirectionalLight()->direction, -360.0f, 360.0f, "%1.f");
    	ImGui::SliderFloat3("Color", (float*)&scene.GetDirectionalLight()->color, 0.0f, 255.0f, "%1.f");
    	ImGui::PopItemWidth();
		
    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawConsole()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		
		Scene& scene = renderer.GetScene();
    	
    	ImGui::Begin("Console", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	
    	
		
    	std::vector<std::string> lines = console_capture->GetLines();
    	for (const std::string& line : lines)
    	{
    		auto [color, text] = ParseAnsiLine(line);
    		ImGui::PushTextWrapPos(0.0f);
    		ImGui::TextColored(color, "%s", text.c_str());
    		ImGui::PopTextWrapPos();
    	}
		
    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawHierarchy()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
    	
    	ImGui::Begin("Hierarchy", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	
    	
    	ImGui::PushStyleColor(ImGuiCol_Header, background_primary);
    	
    	if (ImGui::BeginPopupContextWindow())
    	{
    		if (ImGui::MenuItem("Create Entity"))
    		{
    			CreateEntity();
    			ImGui::CloseCurrentPopup();
    		}
    		ImGui::EndPopup();
    	}
		
    	for (auto& [entity_id, entity] : scene.GetEntities())
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
    	ImGui::PushStyleColor(ImGuiCol_Button, background_primary);
    	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, background_primary);
    	ImGui::PushStyleColor(ImGuiCol_ButtonActive, background_primary);
    	ImGui::Button("##MoveButton", ImVec2(button_width, button_height));

    	if (ImGui::BeginDragDropTarget())
    	{
    		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE"))
    		{
    			uint32_t dragged_entity_id = *(const uint32_t*)payload->Data;
    			if (scene.GetEntities().contains(dragged_entity_id))
    			{
    				std::shared_ptr<Entity> dragged_entity = scene.GetEntities().at(dragged_entity_id);
    				entity_to_reparent = dragged_entity;
    				reparent_target = nullptr;
    			}
    		}
    		ImGui::EndDragDropTarget();
    	}
    	ImGui::PopStyleColor(4);
    	
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
    }

	void Editor::DrawDetails()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
    	
    	ImGui::Begin("Details", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	
    	
		
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
    		if (ImGui::DragFloat3("Position ", (float*)&selected_entity->position, drag_step, -1000.0f, 1000.0f, "%.3f"))
    			selected_entity->UpdateComponents(physics_system);
    		if (ImGui::DragFloat3("Scale ", (float*)&selected_entity->scale, drag_step, 0.01f, 1000.0f, "%.3f"))
    			selected_entity->UpdateComponents(physics_system);
    		if (ImGui::DragFloat3("Rotation ", (float*)&selected_entity->rotation, drag_step, 0.0f, 360.0f, "%.3f"))
    			selected_entity->UpdateComponents(physics_system);
    		ImGui::PopItemWidth();

    		// MODEL COMPONENT
    		if (selected_entity->HasComponent<ModelComponent>())
    		{
    			ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();
    			
    			ImGui::PushID(&model_component);
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

    			if (ImGui::BeginPopup("ChangeModelModelComponent"))
    			{
    				for (auto& [id, scene_item] : scene.GetModels())
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
    				for (auto& [id, scene_item] : scene.GetShaders())
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
    			ImGui::Text("Materials", model_component.materials.size());
    			for (size_t i = 0; i < model_component.materials.size(); i++)
    			{
    				ImGui::PushID(&model_component.materials[i]);
    				std::string label = "Mesh " + std::to_string(i);
    				if (ImGui::Button(model_component.materials[i]->name.c_str(), ImVec2(100, 22)))
    					ImGui::OpenPopup("ChangeMaterialMesh");
    				ImGui::SameLine(); ImGui::Text(label.c_str());

    				if (ImGui::BeginPopup("ChangeMaterialMesh"))
    				{
    					for (auto& [id, mat] : scene.GetMaterials())
    					{
    						if (ImGui::Selectable(mat->name.c_str(), false, 0))
    						{
    							model_component.materials[i] = mat;
    							ImGui::CloseCurrentPopup();
    						}
    					}
    					ImGui::EndPopup();
    				}
    				ImGui::PopID();
    			}

    			ImGui::PopID();
    		}

    		if (selected_entity->HasComponent<LightSourceComponent>())
    		{
    			LightSourceComponent& light_source_component = selected_entity->GetComponent<LightSourceComponent>();
    			
    			ImGui::PushID(&light_source_component);
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
    				ImGui::SliderFloat("Intensity", &point_light->intensity, 0.0f, 10.0f, "%.1f");
    				ImGui::SliderFloat3("Color", (float*)&point_light->color, 0.0f, 255.0f, "%1.f");
    				ImGui::PopItemWidth();
    			}
    			else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
    			{
    				ImGui::Spacing();
    				ImGui::Text("Spot Light Properties");
    				ImGui::PushItemWidth(200.0f);
    				ImGui::SliderFloat3("Color", (float*)&spot_light->color, 0.0f, 255.0f, "%1.f");
    				ImGui::DragFloat3("Scale", (float*)&spot_light->scale, drag_step, 0.0f, 100.0f, "%.1f");
    				ImGui::DragFloat3("Direction", (float*)&spot_light->direction, drag_step, -1.0f, 1.0f, "%.2f");
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
    						scene.GetSpotLights().erase(old_spot->id);

    					std::shared_ptr<PointLight> new_light = std::make_shared<PointLight>();
    					new_light->color = preserved_color;
    					new_light->position = preserved_position;
    					new_light->scale = preserved_scale;
    					new_light->intensity = preserved_intensity;
    					new_light->id = old_light->id;
    					new_light->enabled = old_light->enabled;
    					light_source_component.light_source = new_light;

    					scene.GetPointLights().insert_or_assign(new_light->id, new_light);
    					ImGui::CloseCurrentPopup();
    				}
    				if (ImGui::MenuItem("Spot Light"))
    				{
    					if (auto current_type = std::dynamic_pointer_cast<SpotLight>(old_light))
    						ImGui::CloseCurrentPopup();
    					if (auto old_point = std::dynamic_pointer_cast<PointLight>(old_light))
    						scene.GetPointLights().erase(old_point->id);

    					std::shared_ptr<SpotLight> new_light = std::make_shared<SpotLight>();
    					new_light->color = preserved_color;
    					new_light->position = preserved_position;
    					new_light->scale = preserved_scale;
    					new_light->direction = preserved_direction;
    					new_light->id = old_light->id;
    					new_light->enabled = old_light->enabled;
    					light_source_component.light_source = new_light;

    					scene.GetSpotLights().insert_or_assign(new_light->id, new_light);
    					ImGui::CloseCurrentPopup();
    				}
    				ImGui::EndPopup();
    			}
    			
    			ImGui::PopID();
    		}
    		
    		// PHYSICS COMPONENT
    		if (selected_entity->HasComponent<PhysicsComponent>())
    		{
    			PhysicsComponent& physics_component = selected_entity->GetComponent<PhysicsComponent>();

    			ImGui::PushID(&physics_component);
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

    			ImGui::DragFloat3("Collider Dimensions", (float*)&physics_component.physics_body->GetShapeData().dimensions, drag_step, 0.1f, 100.0f);
				physics_component.physics_body->SetScale(physics_component.physics_body->GetShapeData().dimensions);

    			selected_entity->UpdateComponents(physics_system);
    			
    			ImGui::PopID();
    		}

    		// ANIMATION COMPONENT
    		if (selected_entity->HasComponent<AnimationComponent>())
    		{
				AnimationComponent& animation_component = selected_entity->GetComponent<AnimationComponent>();
    			Animator& animator = *animation_component.animator;

    			ImGui::PushID(&animation_component);
    			ImGui::Separator();
				
    			ImGui::Separator();
    			ImGui::Checkbox("##Enabled", &animation_component.enabled);
    			ImGui::SameLine();
    			ImGui::Text("Animation Component");
    			ImGui::SameLine();
    			ImGui::Text(std::to_string(animation_component.id).c_str());
    			ImGui::Spacing();

    			ImGui::Text("Current Animation: ", animator.GetCurrentAnimationName().c_str());

    			std::string state_text = "STOPPED";
    			if (animator.GetState() == AnimationState::PLAYING)
    				state_text = "PLAYING";
    			else if (animator.GetState() == AnimationState::PAUSED)
    				state_text = "PAUSED";
    			ImGui::SameLine();
    			ImGui::Text("State: %s", state_text.c_str());
    			
    			ImGui::Checkbox("Loop", &animator.GetLoop());
    			ImGui::PushItemWidth(100.0f);
    			ImGui::SliderFloat("Speed", &animator.GetSpeed(), 0.0f, 10.0f);
    			ImGui::PopItemWidth();
    			ImGui::PushItemWidth(300.0f);
    			float progress = animator.GetCurrentAnimation() && animator.GetCurrentAnimation()->GetDuration() > 0.0f ? animator.GetCurrentAnimationTime() / animator.GetCurrentAnimation()->GetDuration() : 0.0f;
    			ImGui::ProgressBar(progress);
    			ImGui::PopItemWidth();

    			if (animator.GetState() == AnimationState::PLAYING)
    			{
    				if (ImGui::Button("Pause"))
    					animator.Pause();
    			}
    			else
    			{
    				if (ImGui::Button("Play"))
    				{
    					if (animator.GetCurrentAnimationName().empty())
    					{
    						if (!animator.GetAnimations().empty())
    						{
    							const auto& first_animation_name = animator.GetAnimations().begin()->first;
    							animator.Play(first_animation_name);
    						}
    					}
    					else
    						animator.Play(animator.GetCurrentAnimationName());
    				}
    			}
    			ImGui::SameLine();
    			if (ImGui::Button("Stop"))
    				animator.Stop();

    			ImGui::Spacing();

    			ImGui::Text("Animations");
    			ImGui::PushItemWidth(100.0f);
    			for (auto& [animation_name, animation] : animator.GetAnimations())
    			{
    				if (ImGui::Button(animation_name.c_str()))
    				{
    					animator.Stop();
    					animator.Play(animation_name);
    				}
    			}
    			ImGui::PopItemWidth();
    			
    			ImGui::PopID();
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
    					CreateModelComponent();
    					ImGui::CloseCurrentPopup();
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
    					CreateLightSourceComponent();
    					ImGui::CloseCurrentPopup();
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
    					CreatePhysicsComponent();
    					ImGui::CloseCurrentPopup();
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
    					CreateAnimationComponent();
    					ImGui::CloseCurrentPopup();
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
    	
    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawParamEditor()
    {
		Project& project = Project::GetInstance();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		ParamDatabase& param_database = renderer.GetParamDatabase();
		
    	
    	ImGui::Begin("Param Editor", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing();
    	
    	
    	
	    if (ImGui::BeginTabBar("ParamEditorTabs"))
		{
			ImGui::PushStyleColor(ImGuiCol_TabActive, highlight_primary);
			ImGui::PushStyleColor(ImGuiCol_TabHovered, highlight_secondary);
			ImGui::PushStyleColor(ImGuiCol_Tab, background_tertiary);
			ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, highlight_primary);
			if (ImGui::BeginTabItem("Model Params"))
		    {
		        for (auto& [model_id, model_data] : param_database.model_params)
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
							default_model_path = abs_str.substr(data_pos);
							std::replace(default_model_path.begin(), default_model_path.end(), '\\', '/');
						}
						else
							default_model_path = model_file;

						Log::Info("File selected at " + default_model_path);
					
						uint32_t next_id = 1000;
						if (!scene.GetModels().empty())
						{
							auto max_it = std::max_element(
								scene.GetModels().begin(),
								scene.GetModels().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
								);
							next_id = max_it->first + 1;
						}

						std::filesystem::path path_obj(default_model_path);
						std::string model_name = path_obj.stem().string();

						bool has_animations = false;
						int animation_count = 0;

						std::filesystem::path absolute_model_path = std::filesystem::absolute(default_model_path);
						std::string abs_path_str = absolute_model_path.string();

						Assimp::Importer temp_importer;
						const aiScene* temp_scene = temp_importer.ReadFile(abs_path_str, aiProcess_ValidateDataStructure | 0);

						if (!temp_scene)
						{
							Log::Warning("Assimp pre-scan failed: " + std::string(temp_importer.GetErrorString()));
							Log::Info("Will attempt to load as regular model");
						}
						else if (temp_scene->HasAnimations() && temp_scene->mNumAnimations > 0)
						{
							has_animations = true;
							animation_count = temp_scene->mNumAnimations;
							Log::Info("Found " + std::to_string(animation_count) + " animation(s)");

							for (unsigned int i = 0; i < temp_scene->mNumAnimations; i++)
							{
								aiAnimation* anim = temp_scene->mAnimations[i];
								std::string anim_name = anim->mName.C_Str();
								if (anim_name.empty())
									anim_name = "Animation_" + std::to_string(i);
								Log::Info("  - " + anim_name + " (" + std::to_string(anim->mDuration) + " ticks, " +
										  std::to_string(anim->mTicksPerSecond) + " tps)");
							}
						}
						else
							Log::Info("No animations found in file");

						std::shared_ptr<Model> new_model;
						if (has_animations)
						{
							Log::Info("Creating SkeletalModel");
							new_model = std::make_shared<SkeletalModel>(default_model_path);
							SkeletalModel* skel_model = static_cast<SkeletalModel*>(new_model.get());
							if (skel_model->GetAnimations().empty())
								Log::Warning("SkeletalModel created but no animations loaded!");
							else
								Log::Info("Successfully loaded " + std::to_string(skel_model->GetAnimations().size()) + " animations");
						}
						else
						{
							Log::Info("Creating regular Model");
							new_model = std::make_shared<Model>(default_model_path);
							new_model->Load();
						}

						new_model->param_id = next_id;
						new_model->name = model_name;

						std::vector<std::shared_ptr<Texture>> auto_textures;
						for (const auto& [tex_path, tex_type] : new_model->extracted_texture_paths)
						{
							std::shared_ptr<Texture> existing_texture = nullptr;
							for (const auto& [id, tex] : scene.GetTextures())
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
								if (!scene.GetTextures().empty())
								{
									auto max_it = std::max_element(
										scene.GetTextures().begin(),
										scene.GetTextures().end(),
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

								scene.GetTextures().insert_or_assign(tex_id, new_texture);
								param_database.texture_params[tex_id] = TextureParamData(tex_name, tex_type, false, tex_path);

								auto_textures.push_back(new_texture);
								Log::Info("Loaded texture: " + tex_name);
							}
						}
						if (!auto_textures.empty())
						{
							uint32_t mat_id = 1000;
							if (!scene.GetMaterials().empty())
							{
								auto max_it = std::max_element(
									scene.GetMaterials().begin(),
									scene.GetMaterials().end(),
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

							scene.GetMaterials().insert_or_assign(mat_id, new_material);

							std::vector<uint32_t> texture_ids;
							for (auto& tex : auto_textures)
							{
								texture_ids.push_back(tex->param_id);
							}
							param_database.material_params[mat_id] = MaterialParamData(mat_name, texture_ids);

							Log::Info("Created material: " + mat_name + " with " + std::to_string(auto_textures.size()) + " texture(s)");
						}
						
						scene.GetModels().insert_or_assign(next_id, new_model);
						param_database.model_params.insert_or_assign(next_id, ModelParamData(model_name, default_model_path, new_model->IsAnimated()));
					}
					else
						Log::Info("File operation cancelled");
				}
				
		        ImGui::EndTabItem();
		    }
		    
		    if (ImGui::BeginTabItem("Texture Params"))
		    {
		        const char* texture_type_names[] = { "Diffuse", "Specular", "Normal", "Height" , "Emission" };
		        for (auto& [texture_id, texture_data] : param_database.texture_params)
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

		            	GLuint texture_gl_id = scene.GetTextures().at(texture_id)->gl_id;
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
		    				default_texture_path = abs_str.substr(data_pos);
		    				std::replace(default_texture_path.begin(), default_texture_path.end(), '\\', '/');
		    			}
		    			else
		    				default_texture_path = texture_file;

		    			Log::Info("File selected at " + default_texture_path);
					
		    			uint32_t next_id = 200001;
		    			if (!scene.GetTextures().empty())
		    			{
		    				auto max_it = std::max_element(
								scene.GetTextures().begin(),
								scene.GetTextures().end(),
								[](const auto& a, const auto& b) { return a.first < b.first; }
								);
		    				next_id = max_it->first + 1;
		    			}

		    			std::filesystem::path path_obj(default_texture_path);
		    			std::string texture_name = path_obj.stem().string();
					
		    			std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(default_texture_path, TextureType::DIFFUSE, false);
		    			new_texture->param_id = next_id;
		    			new_texture->name = texture_name;
		    			new_texture->Load();
		    			scene.GetTextures().insert_or_assign(next_id, new_texture);
		    			param_database.texture_params[next_id] = TextureParamData(texture_name, TextureType::DIFFUSE, false, default_texture_path);
		    		}
		    		else
		    			Log::Info("File operation cancelled");
		    	}
		    	
		        ImGui::EndTabItem();
		    }

			if (ImGui::BeginTabItem("Material Params"))
			{
				for (auto& [material_id, material_data] : scene.GetMaterials())
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
							for (auto& [texture_id, texture_data] : scene.GetTextures())
							{
								ImGui::PushID(&texture_id);
								ImGui::Image((void*)(intptr_t)texture_data->gl_id, ImVec2(25, 25));
								ImGui::SameLine(); 
								if (ImGui::Selectable(texture_data->name.c_str(), false, 0, ImVec2(150, 25)))
								{
									scene.GetMaterials().at(material_id)->AddTexture(scene.GetTextures().at(texture_id));
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
				for (auto& [shader_id, shader_data] : param_database.shader_params)
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
		
    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }
	
	void Editor::DrawEntityTree(std::shared_ptr<Entity> entity)
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
    	ImGui::PushID(&entity->id);
		
    	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    	bool is_selected = (entity == selected_entity);

    	if (is_selected)
    		flags |= ImGuiTreeNodeFlags_Selected;

    	if (entity->children.empty())
    		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    	if (is_selected)
    	{
    		ImGui::PushStyleColor(ImGuiCol_Header, highlight_primary);
    		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, highlight_secondary);
    		ImGui::PushStyleColor(ImGuiCol_HeaderActive, highlight_primary);
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
    			if (scene.GetEntities().contains(dragged_entity_id))
    			{
    				std::shared_ptr<Entity> dragged_entity = scene.GetEntities().at(dragged_entity_id);
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
    			if (scene.GetEntities().contains(child_id))
    				DrawEntityTree(scene.GetEntities()[child_id]);
    		}
    		ImGui::TreePop();
    	}
    	ImGui::PopID();
    }
	
	void Editor::DrawActiveTitleLine(const ImVec4& active_color, const ImVec4& inactive_color, float thickness)
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
    	ImVec4 color = active_color;
    	if (!ImGui::IsWindowFocused())
    		color = inactive_color;

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

	void Editor::CreateModelComponent()
	{
		Scene& scene = Project::GetRenderer().GetScene();
		
		uint32_t next_id = 100001;
		if (!scene.GetModelComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetModelComponents().begin(),
				scene.GetModelComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
				);
			next_id = max_it->first + 1;
		}

		if (!scene.GetModels().empty())
		{
			std::shared_ptr<Model> default_model = scene.GetModels().begin()->second;
			std::shared_ptr<Shader> default_shader = scene.GetShaders().begin()->second;
			std::shared_ptr<Material> default_material = scene.GetMaterials().begin()->second;
			size_t mesh_count = default_model->meshes.size();
			std::vector<std::shared_ptr<Material>> materials_vec(mesh_count, default_material);
			std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(next_id, true, default_model, default_shader, materials_vec);

			scene.GetModelComponents().insert_or_assign(next_id, new_component);
			selected_entity->AddComponent(ComponentType::MODEL, new_component);
		}
		else
			Log::Warning("No models available");
	}
	void Editor::CreateLightSourceComponent()
	{
		Scene& scene = Project::GetRenderer().GetScene();
		
		uint32_t next_id = 100001;
		if (!scene.GetLightSourceComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetLightSourceComponents().begin(),
				scene.GetLightSourceComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}

		uint32_t next_light_id = 1000;
		if (!scene.GetPointLights().empty())
		{
			auto max_it = std::max_element(
				scene.GetPointLights().begin(),
				scene.GetPointLights().end(),
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

		scene.GetLightSourceComponents().insert_or_assign(next_id, new_component);
		scene.GetPointLights().insert_or_assign(next_light_id, new_light);
		selected_entity->AddComponent(ComponentType::LIGHT, new_component);
	}
	void Editor::CreatePhysicsComponent()
	{
		Scene& scene = Project::GetRenderer().GetScene();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		
		uint32_t next_id = 100001;
		if (!scene.GetPhysicsComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetPhysicsComponents().begin(),
				scene.GetPhysicsComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}
    					
		uint32_t next_po_id = 1000;
		if (!scene.GetPhysicsComponents().empty())
		{
			auto max_it = std::max_element(
				scene.GetPhysicsComponents().begin(),
				scene.GetPhysicsComponents().end(),
				[](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_po_id = max_it->first + 1;
		}

		std::string physics_name = "Physics Object";
		PhysicsBodyType body_type = PhysicsBodyType::STATIC;
		PhysicsShapeType shape_type = PhysicsShapeType::BOX;
		glm::vec3 dimensions = glm::vec3(
			(std::max)(selected_entity->scale.x, 0.1f),
			(std::max)(selected_entity->scale.y, 0.1f),
			(std::max)(selected_entity->scale.z, 0.1f)
			);

		std::shared_ptr<PhysicsBody> physics_body = physics_system.CreateBoxBody(selected_entity->position, glm::quat(glm::radians(selected_entity->rotation)), dimensions, body_type);
		physics_body->id = next_po_id;
		physics_body->enabled = true;
		physics_body->SetEnabled(true);
		physics_body->name = physics_name;

		std::shared_ptr<PhysicsComponent> physics_component = std::make_shared<PhysicsComponent>(next_id, true, physics_body);
		scene.GetPhysicsComponents().insert_or_assign(next_id, physics_component);
		selected_entity->AddComponent(ComponentType::PHYSICS, physics_component);
	}
	void Editor::CreateAnimationComponent()
	{
		Scene& scene = Project::GetRenderer().GetScene();
		
		if (!selected_entity->HasComponent<ModelComponent>())
    	{
    		Log::Warning("Entity must have model to add animator");
			ImGui::CloseCurrentPopup();
			return;
    	}
		
		ModelComponent& model_component = selected_entity->GetComponent<ModelComponent>();
		if (!model_component.model->IsAnimated())
		{
			Log::Warning("Model must be animated (skeletal) to add animator");
			ImGui::CloseCurrentPopup();
			return;
		}
		
	    std::shared_ptr<SkeletalModel> skeletal_model = std::static_pointer_cast<SkeletalModel>(model_component.model);

	    if (skeletal_model->GetAnimations().empty())
	    {
	    	Log::Warning("Skeletal model has no animations loaded");
	    	ImGui::CloseCurrentPopup();
	    }
	    else
	    {
	    	uint32_t next_id = 100001;
	    	if (!scene.GetAnimationComponents().empty())
	    	{
	    		auto max_it = std::max_element(
					scene.GetAnimationComponents().begin(),
					scene.GetAnimationComponents().end(),
					[](const auto& a, const auto& b) { return a.first < b.first; }
				);
	    		next_id = max_it->first + 1;
	    	}

	    	std::shared_ptr<Animator> animator = std::make_shared<Animator>(skeletal_model->GetSkeleton());

	    	for (const auto& animation : skeletal_model->GetAnimations())
	    	{
	    		animator->AddAnimation(animation);
	    	}

	    	std::shared_ptr<AnimationComponent> anim_comp = std::make_shared<AnimationComponent>(next_id, true, animator);

	    	scene.GetAnimationComponents().insert_or_assign(next_id, anim_comp);
	    	selected_entity->AddComponent(ComponentType::ANIMATION, anim_comp);

	    	Log::Info("Added Animation Component with " + std::to_string(skeletal_model->GetAnimations().size()) + " animation(s)");

	    	for (const auto& anim : skeletal_model->GetAnimations())
	    	{
	    		Log::Info("  - " + anim->GetName());
	    	}

	    	ImGui::CloseCurrentPopup();
	    }
	}

	void Editor::CreateEntity(std::shared_ptr<Entity> parent)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		uint32_t next_id = 1000001;
		if (!scene.GetEntities().empty())
		{
			auto max_it = std::max_element(
			  scene.GetEntities().begin(),
			  scene.GetEntities().end(),
			  [](const auto& a, const auto& b) { return a.first < b.first; }
			);
			next_id = max_it->first + 1;
		}
		std::shared_ptr<Entity> new_entity = std::make_shared<Entity>(next_id);
		if (parent != nullptr)
		{
			new_entity->parent = parent->id;
			scene.GetEntities().at(parent->id)->AddChild(new_entity->id);
		}
		scene.GetEntities().insert_or_assign(next_id, new_entity);
	}
	void Editor::DuplicateEntity(std::shared_ptr<Entity> entity)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
	    std::function<uint32_t(std::shared_ptr<Entity>)> DuplicateRecursive;
		DuplicateRecursive = [&](std::shared_ptr<Entity> ent) -> uint32_t
		{
			uint32_t next_entity_id = 1000001;
			if (!scene.GetEntities().empty())
			{
				auto max_it = std::max_element(
				  scene.GetEntities().begin(),
				  scene.GetEntities().end(),
				  [](const auto& a, const auto& b) { return a.first < b.first; }
				);
				next_entity_id = max_it->first + 1;
			}

			std::shared_ptr<Entity> duplicated = std::make_shared<Entity>(*ent);
			duplicated->id = next_entity_id;
			std::vector<uint32_t> original_children = duplicated->children;
			duplicated->children.clear();

			if (ent->HasComponent<ModelComponent>())
			{
				auto& original_component = ent->GetComponent<ModelComponent>();

				uint32_t next_comp_id = 100001;
				if (!scene.GetModelComponents().empty())
				{
				  auto max_comp = std::max_element(
				      scene.GetModelComponents().begin(),
				      scene.GetModelComponents().end(),
				      [](const auto& a, const auto& b) { return a.first < b.first; }
				  );
				  next_comp_id = max_comp->first + 1;
				}

				std::shared_ptr<ModelComponent> new_component = std::make_shared<ModelComponent>(
				  next_comp_id,
				  original_component.enabled,
				  original_component.model,
				  original_component.shader,
				  original_component.materials
				);

				scene.GetModelComponents().insert_or_assign(next_comp_id, new_component);
				duplicated->RemoveComponent(ComponentType::MODEL);
				duplicated->AddComponent(ComponentType::MODEL, new_component);
			}
			if (ent->HasComponent<LightSourceComponent>())
            {
                auto& original_component = ent->GetComponent<LightSourceComponent>();

                // Generate new component ID
                uint32_t next_comp_id = 100001;
                if (!scene.GetLightSourceComponents().empty())
                {
                        auto max_comp = std::max_element(
                                scene.GetLightSourceComponents().begin(),
                                scene.GetLightSourceComponents().end(),
                                [](const auto& a, const auto& b) { return a.first < b.first; }
                        );
                        next_comp_id = max_comp->first + 1;
                }

                uint32_t next_light_id = 1000;

                std::shared_ptr<LightSource> new_light_source;

                if (auto point_light = std::dynamic_pointer_cast<PointLight>(original_component.light_source))
                {
                        if (!scene.GetPointLights().empty())
                        {
                                auto max_light = std::max_element(
                                        scene.GetPointLights().begin(),
                                        scene.GetPointLights().end(),
                                        [](const auto& a, const auto& b) { return a.first < b.first; }
                                );
                                next_light_id = max_light->first + 1;
                        }

                        std::shared_ptr<PointLight> new_point_light = std::make_shared<PointLight>();
                        new_point_light->id = next_light_id;
                        new_point_light->enabled = point_light->enabled;
                        new_point_light->position = point_light->position;
                        new_point_light->color = point_light->color;
                        new_point_light->scale = point_light->scale;

                        new_light_source = new_point_light;
                        scene.GetPointLights().insert_or_assign(next_light_id, new_point_light);
                }
                else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(original_component.light_source))
                {
                        if (!scene.GetSpotLights().empty())
                        {
                                auto max_light = std::max_element(
                                        scene.GetSpotLights().begin(),
                                        scene.GetSpotLights().end(),
                                        [](const auto& a, const auto& b) { return a.first < b.first; }
                                );
                                next_light_id = max_light->first + 1;
                        }

                        std::shared_ptr<SpotLight> new_spot_light = std::make_shared<SpotLight>();
                        new_spot_light->id = next_light_id;
                        new_spot_light->enabled = spot_light->enabled;
                        new_spot_light->position = spot_light->position;
                        new_spot_light->color = spot_light->color;
                        new_spot_light->scale = spot_light->scale;
                        new_spot_light->direction = spot_light->direction;

                        new_light_source = new_spot_light;
                        scene.GetSpotLights().insert_or_assign(next_light_id, new_spot_light);
                }

                std::shared_ptr<LightSourceComponent> new_component = std::make_shared<LightSourceComponent>(
                        next_comp_id,
                        original_component.enabled,
                        new_light_source
                );

                scene.GetLightSourceComponents().insert_or_assign(next_comp_id, new_component);
                duplicated->RemoveComponent(ComponentType::LIGHT);
                duplicated->AddComponent(ComponentType::LIGHT, new_component);
            }
			if (ent->HasComponent<PhysicsComponent>())
		    {
		        PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		        auto& original_component = ent->GetComponent<PhysicsComponent>();
  
		        uint32_t next_comp_id = 100001;
		        if (!scene.GetPhysicsComponents().empty())
		        {
		            auto max_comp = std::max_element(
		                scene.GetPhysicsComponents().begin(),
		                scene.GetPhysicsComponents().end(),
		                [](const auto& a, const auto& b) { return a.first < b.first; }
		            );
		            next_comp_id = max_comp->first + 1;
		        }

		        uint32_t next_physics_id = 1000;
		        if (!scene.GetPhysicsComponents().empty())
		        {
		            uint32_t max_physics_id = 1000;
		            for (const auto& [id, comp] : scene.GetPhysicsComponents())
		            {
		                if (comp->physics_body && comp->physics_body->id > max_physics_id)
		                    max_physics_id = comp->physics_body->id;
		            }
		            next_physics_id = max_physics_id + 1;
		        }

		        PhysicsBodyType body_type = original_component.physics_body->GetBodyType();
		        PhysicsShapeData shape_data = original_component.physics_body->GetShapeData();
		        glm::vec3 position = original_component.physics_body->GetPosition();
		        glm::quat rotation = original_component.physics_body->GetRotation();
		        bool enabled = original_component.physics_body->enabled;
		        std::string name = original_component.physics_body->name;

		        std::shared_ptr<PhysicsBody> new_physics_body;

		        if (shape_data.type == PhysicsShapeType::BOX)
		            new_physics_body = physics_system.CreateBoxBody(position, rotation, shape_data.dimensions, body_type);
		        else if (shape_data.type == PhysicsShapeType::SPHERE)
		            new_physics_body = physics_system.CreateSphereBody(position, shape_data.dimensions.x, body_type);
		        else if (shape_data.type == PhysicsShapeType::CAPSULE)
		            new_physics_body = physics_system.CreateCapsuleBody(position, rotation, shape_data.dimensions.x, shape_data.dimensions.y, body_type);

		        if (new_physics_body)
		        {
		            new_physics_body->id = next_physics_id;
		            new_physics_body->enabled = enabled;
		            new_physics_body->SetEnabled(enabled);
		            new_physics_body->name = name;

		            std::shared_ptr<PhysicsComponent> new_component = std::make_shared<PhysicsComponent>(
		                next_comp_id,
		                original_component.enabled,
		                new_physics_body
		                );

		            scene.GetPhysicsComponents().insert_or_assign(next_comp_id, new_component);
		            duplicated->RemoveComponent(ComponentType::PHYSICS);
		            duplicated->AddComponent(ComponentType::PHYSICS, new_component);
		        }
		    }
			
		    scene.GetEntities().insert_or_assign(next_entity_id, duplicated);

		    for (uint32_t child_id : original_children)
		    {
			    if (scene.GetEntities().contains(child_id))
			    {
				    uint32_t new_child_id = DuplicateRecursive(scene.GetEntities().at(child_id));
				    duplicated->children.push_back(new_child_id);
				    scene.GetEntities().at(new_child_id)->parent = next_entity_id;
			    }
		    }

		    return next_entity_id;
		};

		uint32_t original_parent = entity->parent;
		bool original_is_root = entity->IsRoot();
		uint32_t new_root_id = DuplicateRecursive(entity);

		scene.GetEntities().at(new_root_id)->parent = original_parent;

		if (!original_is_root && scene.GetEntities().contains(original_parent))
			scene.GetEntities().at(original_parent)->children.push_back(new_root_id);

		selected_entity = scene.GetEntities().at(new_root_id);
	}

	void Editor::DeleteEntity(std::shared_ptr<Entity> entity)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		std::function<void(std::shared_ptr<Entity>)> DeleteRecursive;
		DeleteRecursive = [&](std::shared_ptr<Entity> ent)
		{
			std::vector<uint32_t> children_copy = ent->children;
			for (uint32_t child_id : children_copy)
			{
				if (scene.GetEntities().contains(child_id))
				{
					DeleteRecursive(scene.GetEntities()[child_id]);
				}
			}

			if (ent->HasComponent<ModelComponent>())
			{
				auto& model_component = ent->GetComponent<ModelComponent>();
				scene.GetModelComponents().erase(model_component.id);
			}
			if (ent->HasComponent<LightSourceComponent>())
			{
				auto& light_source_component = ent->GetComponent<LightSourceComponent>();
				if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_source_component.light_source))
				{
					scene.GetPointLights().erase(point_light->id);
				}
				if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_source_component.light_source))
				{
					scene.GetSpotLights().erase(spot_light->id);
				}
				scene.GetLightSourceComponents().erase(light_source_component.id);
			}
			if (ent->HasComponent<PhysicsComponent>())
			{
				auto& physics_component = ent->GetComponent<PhysicsComponent>();
				PhysicsSystem& physics_system = Project::GetPhysicsSystem();
				JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
    
				if (physics_component.physics_body != nullptr)
				{
					JPH::BodyID body_id = physics_component.physics_body->GetBodyID();
					if (body_interface.IsAdded(body_id))
					{
						body_interface.RemoveBody(body_id);
						body_interface.DestroyBody(body_id);
					}
				}
    
				scene.GetPhysicsComponents().erase(physics_component.id);
			}
			if (ent->HasComponent<AnimationComponent>())
			{
				auto& animation_component = ent->GetComponent<AnimationComponent>();
				// delete animation component
			}

			if (selected_entity == ent)
				selected_entity = nullptr;

			scene.GetEntities().erase(ent->id);
		};

		if (!entity->IsRoot() && scene.GetEntities().contains(entity->parent))
		{
			scene.GetEntities()[entity->parent]->RemoveChild(entity->id);
		}

		DeleteRecursive(entity);

		if (selected_entity == nullptr && !scene.GetEntities().empty())
			selected_entity = scene.GetEntities().begin()->second;
	}
	
	bool Editor::IsDescendentOf(std::shared_ptr<Entity> potential_child, std::shared_ptr<Entity> potential_parent)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		if (potential_parent == nullptr)
			return false;
		if (potential_parent == potential_child)
			return true;

		for (uint32_t child_id : potential_child->children)
		{
			if (scene.GetEntities().contains(child_id))
			{
				if (IsDescendentOf(scene.GetEntities().at(child_id), potential_parent))
					return true;
			}
		}

		return false;
	}
	void Editor::ReparentEntity(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> new_parent)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		glm::mat4 world_transform = entity->GetWorldTransformMatrix(scene.GetEntities());
		
		if (!entity->IsRoot() && scene.GetEntities().contains(entity->parent))
			scene.GetEntities().at(entity->parent)->RemoveChild(entity->id);

		if (new_parent != nullptr)
		{
			entity->parent = new_parent->id;
			new_parent->AddChild(entity->id);

			glm::mat4 parent_world_transform = new_parent->GetWorldTransformMatrix(scene.GetEntities());
			glm::mat4 new_local_transform = glm::inverse(parent_world_transform) * world_transform;

			glm::vec3 translation, rotation, scale;
			DecomposeTransform(new_local_transform, translation, rotation, scale);
			entity->position = translation;
			entity->rotation = glm::degrees(rotation);
			entity->scale = scale;
		}
		else
		{
			entity->parent = 0;

			glm::vec3 translation, rotation, scale;
			DecomposeTransform(world_transform, translation, rotation, scale);
			entity->position = translation;
			entity->rotation = glm::degrees(rotation);
			entity->scale = scale;
		}
	}
}
