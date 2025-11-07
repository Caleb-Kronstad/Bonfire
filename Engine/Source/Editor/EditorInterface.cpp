#include "bonfire_pch.hpp"
#include "Editor.hpp"

#include "Commands.hpp"
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
		
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (project.GetEngineRunState())
		{
			window_flags |= ImGuiWindowFlags_MenuBar;
			ImGui::Begin("DockSpace", nullptr, window_flags);
			ImGui::PopStyleVar(2);
	
			DrawMenuBar();
			DrawEditorViewport();
			DrawProjectViewport(0);
			DrawToolbar();
			DrawDebugInfo();
			DrawConsole();
			DrawProjectSettings();
			DrawParamEditor();
			DrawHierarchy();
			DrawDetails();
		}
		else
		{
			DrawProjectViewport(window_flags);
			ImGui::PopStyleVar(2);
		}
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

		ImGui::PushFont(editor_font);
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
	            if (ImGui::MenuItem("Build"))
	            {
	            	BuildProject();
	            }
	            
	            ImGui::Separator();
	            if (ImGui::MenuItem("Exit", "Alt+F4")) { project.SetEngineRunState(false); }
	            ImGui::EndMenu();
	        }
	        
	        if (ImGui::BeginMenu("Edit"))
	        {
	            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
	            {
	            	if (command_history->CanUndo())
	            		command_history->Undo();
	            }
	            if (ImGui::MenuItem("Redo", "Ctrl+Y"))
	            {
		            if (command_history->CanRedo())
		            	command_history->Redo();
	            }
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

		ImGui::PopFont();
	    ImGui::End();
	}
	
	void Editor::DrawEditorViewport()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		PhysicsSystem& physics_system = project.GetPhysicsSystem();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		ImGui::PushFont(editor_font);
		editor_viewport_visible = ImGui::Begin("Viewport");
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
		
		if (!editor_viewport_visible)
		{
			ImGui::PopFont();
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
    			if (!is_modifying_transform)
    			{
    				transform_start_position = selected_entity->position;
    				transform_start_rotation = selected_entity->rotation;
    				transform_start_scale = selected_entity->scale;
    				is_modifying_transform = true;
    			}
    			
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
    			selected_entity->UpdateComponents();
    		}
    		else if (is_modifying_transform && (!ImGuizmo::IsUsing() || ImGui::IsMouseReleased(ImGuiMouseButton_Left)))
    		{
    			std::shared_ptr<SetTransformCommand> command = std::make_unique<SetTransformCommand>(
    				&scene, &physics_system, selected_entity->id, transform_start_position, transform_start_rotation, transform_start_scale,
    				selected_entity->position, selected_entity->rotation, selected_entity->scale);
    			command_history->ExecuteCommand(command);
    			is_modifying_transform = false;
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

		ImGui::PopFont();
    	ImGui::End();
    }

	void Editor::DrawProjectViewport(ImGuiWindowFlags window_flags)
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		ImGui::PushFont(editor_font);
		project_viewport_visible = ImGui::Begin("Project Name Here", nullptr, window_flags);
		if (project.GetEngineRunState())
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

		ImGui::PopFont();
		ImGui::End();
	}

	void Editor::DrawDebugInfo()
	{
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
    	
		ImGui::PushFont(editor_font);
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

		if (ImGui::Checkbox("Preview Animations", &preview_animations))
		{
			if (!preview_animations)
			{
				for (auto& [component_id, component] : scene.GetAnimationComponents())
					component->animator->Stop();
			}
		}
		if (ImGui::Checkbox("Preview Audios", &preview_audios))
		{
			if (!preview_audios)
			{
				for (auto& [component_id, component] : scene.GetAudioComponents())
					component->audio->Stop();
			}
			else
			{
				for (auto& [component_id, component] : scene.GetAudioComponents())
				{
					if (component->audio->GetPlayOnAwake() && !component->audio->IsPlaying())
						component->audio->Play();
				}
			}
		}

		ImGui::Checkbox("Draw Colliders", &renderer.GetDrawColliders());
		ImGui::SliderFloat("Collider Line Width", &renderer.GetDrawCollidersLineWidth(), 0.1f, 10.0f, "%.1f");

    	const char* debug_type_names[] = { "DEFAULT", "WIREFRAME", "POINT" };
    	int current_debug_type = static_cast<int>(renderer.GetDebugType());
    	if (ImGui::Combo("Debug Mode", &current_debug_type, debug_type_names, IM_ARRAYSIZE(debug_type_names)))
    	{
    		renderer.GetDebugType() = static_cast<DebugType>(current_debug_type);
    	}

		ImGui::PopFont();
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
    	
		ImGui::PushFont(editor_font);
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
    			for (std::shared_ptr<Layer> layer : project.GetLayers())
    				layer->OnAttach();
    			project.SetProjectRunState(true);
    			selected_entity = nullptr;
    			Project::GetScriptSystem().StartScripts(scene);
    			ImGui::SetWindowFocus("Project Name Here");
    		}
    		// stop playing
    		else if (project.GetProjectRunState())
    		{
    			Log::Info("Stopping...");
    			project.SetProjectRunState(false);
    			
    			for (auto& [entity_id, entity] : scene.GetEntities())
    			{
    				if (entity->HasComponent<AnimationComponent>() && !preview_animations)
    				{
    					AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
    					if (animation_component.animator)
    						animation_component.animator->Stop();
    				}
    				if (entity->HasComponent<AudioComponent>() && !preview_audios)
    				{
    					AudioComponent& audio_component = entity->GetComponent<AudioComponent>();
    					if (audio_component.audio && audio_component.enabled)
    					{
    						audio_component.audio->Set3DPosition(entity->position);
    						if (audio_component.audio->GetPlayOnAwake() && !audio_component.audio->IsPlaying())
    							audio_component.audio->Stop();
    					}
    				}
    			}
    			
    			Project::GetScriptSystem().DestroyScripts(scene);
    			for (std::shared_ptr<Layer> layer : project.GetLayers())
    				layer->OnDetach();
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

		ImGui::PopFont();    	
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawProjectSettings()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		ImGui::PushFont(editor_font);
    	ImGui::Begin("Project Settings", nullptr);
    	DrawActiveTitleLine(highlight_primary, background_tertiary);
    	ImGui::Indent(8.0f);
    	ImGui::Spacing(); 

		ImGui::PushItemWidth(100.0f);
    	ImGui::DragFloat("Drag Step", &drag_step, 0.1f, 0.1f, 100.0f, "%.2f");
		int temp_undo_redo_steps = undo_redo_steps;
		ImGui::SliderInt("Undo/Redo Steps", &temp_undo_redo_steps, 1, 256);
		undo_redo_steps = (uint8_t)temp_undo_redo_steps;

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
		
    	ImGui::PopFont();
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawConsole()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		
		Scene& scene = renderer.GetScene();
    	
		ImGui::PushFont(editor_font);
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
		
    	ImGui::PopFont();
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawHierarchy()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		ImGui::PushFont(editor_font);
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

		ImGui::PopFont();
    	ImGui::Unindent(8.0f);
    	ImGui::End();
    }

	void Editor::DrawDetails()
    {
		Project& project = Project::GetInstance();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		
		ImGui::PushFont(editor_font);
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
    			selected_entity->UpdateComponents();
    		if (ImGui::DragFloat3("Scale ", (float*)&selected_entity->scale, drag_step, 0.01f, 1000.0f, "%.3f"))
    			selected_entity->UpdateComponents();
    		if (ImGui::DragFloat3("Rotation ", (float*)&selected_entity->rotation, drag_step, 0.0f, 360.0f, "%.3f"))
    			selected_entity->UpdateComponents();
    		ImGui::PopItemWidth();

    		DisplayCameraComponent();
    		DisplayModelComponent();
    		DisplayPhysicsComponent();
    		DisplayAnimationComponent();
    		DisplayLightSourceComponent();
    		DisplayAudioComponent();
    		DisplayScriptComponent();

    		ImGui::Separator();
    		
    		// ADD COMPONENT
    		ImGui::PushID("##ADDCOMPONENT");
		
    		if (ImGui::Button("Add Component"))
    			ImGui::OpenPopup("AddComponentPopup");

    		if (ImGui::BeginPopup("AddComponentPopup"))
    		{
    			if (ImGui::MenuItem("Camera Component"))
    			{
    				if (!selected_entity->HasComponent<CameraComponent>())
    				{
    					CreateCameraComponent(selected_entity);
    					ImGui::CloseCurrentPopup();
    				}
				    else
				    {
					    Log::Warning("Entity already has camera component");
				    	ImGui::CloseCurrentPopup();
				    }
    			}
    			
    			if (ImGui::MenuItem("Model Component"))
    			{
    				if (!selected_entity->HasComponent<ModelComponent>())
    				{
    					CreateModelComponent(selected_entity);
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
    					CreateLightSourceComponent(selected_entity);
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
    					CreatePhysicsComponent(selected_entity);
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
    					CreateAnimationComponent(selected_entity);
    					ImGui::CloseCurrentPopup();
    				}
    			  	else
    			  	{
    			  		Log::Warning("Entity already has animation component");
    			  		ImGui::CloseCurrentPopup();
    			  	}
    			}

    			if (ImGui::MenuItem("Audio Component"))
    			{
    				if (!selected_entity->HasComponent<AudioComponent>())
    				{
    					CreateAudioComponent(selected_entity);
    					ImGui::CloseCurrentPopup();
    				}
    				else
    				{
    					Log::Warning("Entity already has audio component");
    					ImGui::CloseCurrentPopup();
    				}
    			}

    			if (ImGui::MenuItem("Script Component"))
    			{
    				if (!selected_entity->HasComponent<ScriptComponent>())
    				{
    					CreateScriptComponent(selected_entity);
    					ImGui::CloseCurrentPopup();
    				}
				    else
				    {
					    Log::Warning("Entity already has script component");
				    	ImGui::CloseCurrentPopup();
				    }
    			}
    			ImGui::EndPopup();
    		}
    	
    		ImGui::PopID();

    		ImGui::Spacing();
    		
    		ImGui::PushID("##REMOVECOMPONENT");

    		if (ImGui::Button("Remove Component"))
    			ImGui::OpenPopup("RemoveComponentPopup");

    		if (ImGui::BeginPopup("RemoveComponentPopup"))
    		{
    			selected_entity_to_remove_components = selected_entity;
    			
    			if (ImGui::MenuItem("Camera Component"))
    			{
    				if (selected_entity->HasComponent<CameraComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::CAMERA;
    					ImGui::CloseCurrentPopup();
    				}
				    else
				    {
					    Log::Warning("Entity does not have a camera component to remove");
				    	ImGui::CloseCurrentPopup();
				    }
    			}
    			
    			if (ImGui::MenuItem("Model Component"))
    			{
    				if (selected_entity->HasComponent<ModelComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::MODEL;
    					ImGui::CloseCurrentPopup();
    				}
    				else
    				{
					    Log::Warning("Entity does not have a model component to remove");
    					ImGui::CloseCurrentPopup();
    				}
    			}

    			if (ImGui::MenuItem("Light Source Component"))
    			{
    				if (selected_entity->HasComponent<LightSourceComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::LIGHT;
    					ImGui::CloseCurrentPopup();
    				}
    				else
    				{
					    Log::Warning("Entity does not have a light source component to remove");
    					ImGui::CloseCurrentPopup();
    				}
    			}
    			
    			if (ImGui::MenuItem("Physics Component"))
    			{
    				if (selected_entity->HasComponent<PhysicsComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::PHYSICS;
    					ImGui::CloseCurrentPopup();
    				}
    				else
    				{
					    Log::Warning("Entity does not have a physics component to remove");
    					ImGui::CloseCurrentPopup();
    				}
    			}

    			if (ImGui::MenuItem("Animation Component"))
    			{
    				if (selected_entity->HasComponent<AnimationComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::ANIMATION;
    					ImGui::CloseCurrentPopup();
    				}
    			  	else
    			  	{
					    Log::Warning("Entity does not have a animation component to remove");
    			  		ImGui::CloseCurrentPopup();
    			  	}
    			}

    			if (ImGui::MenuItem("Audio Component"))
    			{
    				if (selected_entity->HasComponent<AudioComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::AUDIO;
    					ImGui::CloseCurrentPopup();
    				}
    				else
    				{
					    Log::Warning("Entity does not have a audio component to remove");
    					ImGui::CloseCurrentPopup();
    				}
    			}

    			if (ImGui::MenuItem("Script Component"))
    			{
    				if (selected_entity->HasComponent<ScriptComponent>())
    				{
    					selected_entity_to_remove_components = selected_entity;
    					component_to_remove = ComponentType::SCRIPT;
    					ImGui::CloseCurrentPopup();
    				}
				    else
				    {
					    Log::Warning("Entity does not have a script component to remove");
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
    }

	void Editor::DrawParamEditor()
    {
		Project& project = Project::GetInstance();
		PhysicsSystem& physics_system = Project::GetPhysicsSystem();
		Window& project_window = project.GetWindow();
		Renderer& renderer = project.GetRenderer();
		Scene& scene = renderer.GetScene();
		ParamDatabase& param_database = renderer.GetParamDatabase();
		
		ImGui::PushFont(editor_font);
    	ImGui::Begin("Params", nullptr);
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
		    				default_diffuse_path = abs_str.substr(data_pos);
		    				std::replace(default_diffuse_path.begin(), default_diffuse_path.end(), '\\', '/');
		    			}
		    			else
		    				default_diffuse_path = texture_file;

		    			Log::Info("File selected at " + default_diffuse_path);
					
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

		    			std::filesystem::path path_obj(default_diffuse_path);
		    			std::string texture_name = path_obj.stem().string();
					
		    			std::shared_ptr<Texture> new_texture = std::make_shared<Texture>(default_diffuse_path, TextureType::DIFFUSE, false);
		    			new_texture->param_id = next_id;
		    			new_texture->name = texture_name;
		    			new_texture->Load();
		    			scene.GetTextures().insert_or_assign(next_id, new_texture);
		    			param_database.texture_params[next_id] = TextureParamData(texture_name, TextureType::DIFFUSE, false, default_diffuse_path);
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
						ImGui::InputText("##1400", &material_data->name);
						ImGui::SameLine(); ImGui::Text(std::to_string(material_data->param_id).c_str());
						ImGui::SliderFloat("Shininess", &material_data->shininess, 0.0f, 512.0f, "%.f");
						ImGui::SliderFloat2("Tiling", (float*)&material_data->texture_tiling, 1.0f, 100.0f, "%.f");
						ImGui::SliderFloat2("Offset", (float*)&material_data->texture_offset, 0.1f, 10.0f, "%.2f");

						ImGui::Spacing();
						ImGui::Text("Textures");
						for (auto& texture : material_data->textures)
						{
							ImGui::PushID(&texture);
							ImGui::Text(texture->name.c_str());
							ImGui::SameLine(); ImGui::Text(std::to_string(texture->param_id).c_str());
							
							if (ImGui::ImageButton((void*)texture->gl_id, ImVec2(100,100)))
							{
								ImGui::OpenPopup("ChangeMaterialTexture");
							}
							if (ImGui::BeginPopup("ChangeMaterialTexture"))
							{
								for (auto& [texture_id, list_texture] : scene.GetTextures())
								{
									if (texture->type != list_texture->type) continue;
									ImGui::PushID(&texture_id);
									ImGui::Image((void*)(intptr_t)list_texture->gl_id, ImVec2(20, 20));
									ImGui::SameLine();
									if (ImGui::Selectable(list_texture->name.c_str(), false, 0, ImVec2(150, 20)))
									{
										texture = list_texture;
										ImGui::CloseCurrentPopup();
									}

									ImGui::PopID();
								}
								ImGui::EndPopup();
							}
							ImGui::PopID();
						}
					}
					ImGui::PopID();
				}

				ImGui::Separator();

				if (ImGui::Button("+"))
				{
					uint32_t next_id = 1000;
					if (!scene.GetMaterials().empty())
					{
						auto max_it = std::max_element(
							scene.GetMaterials().begin(),
							scene.GetMaterials().end(),
							[](const auto& a, const auto& b) { return a.first < b.first; }
							);
						next_id = max_it->first + 1;
					}

					std::shared_ptr<Material> new_material = std::make_shared<Material>("New Material");
					new_material->param_id = next_id;
					new_material->AddTexture(default_textures.at(0));
					new_material->AddTexture(default_textures.at(1));
					new_material->AddTexture(default_textures.at(2));
					new_material->AddTexture(default_textures.at(3));
					new_material->AddTexture(default_textures.at(4));
					new_material->shininess = 64.0f;
					new_material->texture_tiling = glm::vec2(1.0f, 1.0f);
					new_material->texture_offset = glm::vec2(0.0f, 0.0f);

					scene.GetMaterials().insert_or_assign(next_id, new_material);
					param_database.material_params.insert_or_assign(
						next_id,
						MaterialParamData(new_material->name,
						new_material->GetTexture(TextureType::DIFFUSE)->param_id, new_material->GetTexture(TextureType::SPECULAR)->param_id,new_material->GetTexture(TextureType::NORMAL)->param_id,
						new_material->GetTexture(TextureType::HEIGHT)->param_id, new_material->GetTexture(TextureType::EMISSION)->param_id,
						new_material->shininess, new_material->texture_tiling, new_material->texture_offset)
						);
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
						ImGui::SetNextItemWidth(200.0f);
						ImGui::InputText("Name", &shader_data.name);
						ImGui::Text(("Vertex Shader: " + shader_data.vert_path).c_str());
						ImGui::Text(("Fragment Shader: " + shader_data.frag_path).c_str());
						ImGui::Text(("Geometry Shader: " + shader_data.geom_path).c_str());
					}
					ImGui::PopID();
				}
				
		        ImGui::EndTabItem();
			}

	    	if (ImGui::BeginTabItem("Audio Params"))
	    	{
	    		for (auto& [audio_id, audio_data] : param_database.audio_params)
	    		{
	    			ImGui::PushID(&audio_data);
	    			if (ImGui::CollapsingHeader(std::to_string(audio_id).c_str()))
	    			{
						ImGui::SetNextItemWidth(200.0f);
						ImGui::InputText("Name: ", &audio_data.name);
	    				ImGui::Text("Path: %s", audio_data.path.c_str());
	    			}
					ImGui::PopID();
	    		}

	    		ImGui::Separator();

				char exe_path[MAX_PATH];
	    		GetModuleFileNameA(NULL, exe_path, MAX_PATH);
	    		std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
	    		std::filesystem::path audio_dir = exe_dir / "Data/Resources/Audio";
	    		std::string audio_file = std::string(MAX_PATH, '\0');

	    		if (ImGui::Button("+"))
	    		{
	    			OPENFILENAMEA ofn;
	    			ZeroMemory(&ofn, sizeof(OPENFILENAME));
	    			ofn.lStructSize = sizeof(OPENFILENAME);
	    			ofn.lpstrFile = (LPSTR)audio_file.c_str();
	    			ofn.nMaxFile = audio_file.size();
	    			ofn.lpstrInitialDir = audio_dir.string().c_str();
	    			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	    			ofn.lpstrFilter = "Audio Files\0*.wav;*.mp3;*.ogg;*.flac\0WAV Files\0*.wav\0MP3 Files\0*.mp3\0OGG Files\0*.ogg\0FLAC Files\0*.flac\0All Files\0*.*\0";
	    			ofn.lpstrTitle = "Select audio file";

	    			if (GetOpenFileNameA(&ofn))
	    			{
	    				audio_file.resize(audio_file.find('\0'));
	    				std::filesystem::path absolute_path = audio_file;
	    				std::string abs_str = absolute_path.string();
	    				std::string relative_audio_path;
	    				size_t data_pos = abs_str.find("Data");
	    				if (data_pos != std::string::npos)
	    				{
	    					relative_audio_path = abs_str.substr(data_pos);
	    					std::replace(relative_audio_path.begin(), relative_audio_path.end(), '\\', '/');
	    				}
	    				else
	    					relative_audio_path = audio_file;

	    				Log::Info("Audio file selected at " + relative_audio_path);
	    				uint32_t next_id = 1000;
	    				AudioSystem& audio_system = Project::GetAudioSystem();
	    				if (!audio_system.GetAudios().empty())
	    				{
	    					auto max_it = std::max_element(
									audio_system.GetAudios().begin(),
									audio_system.GetAudios().end(),
									[](const auto& a, const auto& b) { return a.first < b.first; }
							);
	    					next_id = max_it->first + 1;
	    				}

	    				std::filesystem::path path_obj(relative_audio_path);
	    				std::string audio_name = path_obj.stem().string();

	    				std::shared_ptr<Audio> new_audio = std::make_shared<Audio>(next_id, audio_name, relative_audio_path);
	    				audio_system.AddAudio(new_audio);
	    				param_database.audio_params.insert_or_assign(next_id, AudioParamData(audio_name, relative_audio_path));
	    				Log::Info("Loaded audio " + audio_name);
	    			}
	    			else
	    				Log::Info("Audio file operation cancelled");
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
    			entity_to_remove = entity;
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

	void Editor::BuildProject()
	{
		Project& project = Project::GetInstance();
		
		try
        {
            std::string project_name = project.GetProjectConfig().project_name;
            if (project_name.empty())
            {
                project_name = "Bonfire Project";
            }

			std::string project_name_no_spaces = project_name;
			project_name_no_spaces.erase(std::remove_if(project_name_no_spaces.begin(), project_name_no_spaces.end(), isspace), project_name_no_spaces.end());
  
            std::filesystem::path exe_dir = std::filesystem::current_path();
            std::filesystem::path builds_dir = exe_dir / "Builds";
            std::filesystem::create_directories(builds_dir);
            std::filesystem::path build_dir = builds_dir / project_name_no_spaces;
  
            if (std::filesystem::exists(build_dir))
            {
                std::filesystem::remove_all(build_dir);
            }
            std::filesystem::create_directories(build_dir);
  
            Log::Info("[EditorInterface] Building project to: " + build_dir.string());

            std::filesystem::path exe_source = exe_dir / "Bonfire.exe";
            std::filesystem::path exe_dest = build_dir / (project_name_no_spaces + ".exe");
            if (std::filesystem::exists(exe_source))
            {
                std::filesystem::copy(exe_source, exe_dest, std::filesystem::copy_options::overwrite_existing);
            }
            else
            {
                Log::Error("[EditorInterface] Bonfire.exe not found");
                return;
            }
  
            std::filesystem::path data_source = exe_dir / "Data";
            std::filesystem::path data_dest = build_dir / "Data";
            if (std::filesystem::exists(data_source))
            {
                std::filesystem::copy(data_source, data_dest, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
            }
            else
            {
                Log::Error("[EditorInterface] Data folder not found");
                return;
            }
  
            std::filesystem::path imgui_source = exe_dir / "imgui.ini";
            std::filesystem::path imgui_dest = build_dir / "imgui.ini";
            if (std::filesystem::exists(imgui_source))
            {
                std::filesystem::copy(imgui_source, imgui_dest, std::filesystem::copy_options::overwrite_existing);
            }
            else
            {
	            Log::Warning("[EditorInterface] imgui.ini not found");
            }
  
            std::filesystem::path config_path = data_dest / "projectconfig.bonfire";
            if (std::filesystem::exists(config_path))
            {
                std::ifstream config_file(config_path);
                if (config_file.is_open())
                {
                    nlohmann::json config_json;
                    config_file >> config_json;
                    config_file.close();
  
                    config_json["enable-editor"] = false;

                    std::ofstream config_out(config_path);
                    if (config_out.is_open())
                    {
                        config_out << std::setw(4) << config_json << std::endl;
                        config_out.close();
                        Log::Info("[EditorInterface] Disabled editor in build config");
                    }
                    else
                    {
                        Log::Error("[EditorInterface] Failed to write projectconfig.bonfire");
                    }
                }
                else
                {
                    Log::Error("[EditorInterface] Failed to read projectconfig.bonfire");
                }
            }
            else
            {
                Log::Warning("[EditorInterface] projectconfig.bonfire not found in Data folder");
            }

            Log::Info("[EditorInterface] Build completed successfully");
            Log::Info("[EditorInterface] Output: " + build_dir.string());
        }
        catch (const std::exception& e)
        {
            Log::Error("[EditorInterface] Build failed: " + std::string(e.what()));
        }
	}
}
