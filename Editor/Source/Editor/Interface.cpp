#include "Editor.hpp"

void Editor::OnInterfaceUpdate()
{
	Engine& engine = Engine::Instance();
	Renderer& renderer = Engine::GetRenderer();
	Scene& scene = renderer.GetScene();

	if (EditorViewportVisible())
		renderer.RenderViewport(engine.GetDeltaTime(), *engine_camera, *editor_viewport_framebuffer, editor_viewport_size);

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

	if (engine.GetEditorRunState())
	{
		window_flags |= ImGuiWindowFlags_MenuBar;
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		DrawMenuBar();

		switch (view)
		{
		case 0:
			DrawEditorViewport();
			DrawProjectViewport(0);
			DrawToolbar();
			DrawDebugInfo();
			DrawConsole("Console");
			DrawProjectSettings();
			DrawHierarchy();
			DrawDetails();
			break;
		case 1:
			DrawParamEditor();
			//DrawConsole("Console##Param");
			break;
		default:
			break;
		}
	}
}

void Editor::UpdateEditorInterfaceStyle()
{
	text_primary = RgbaToImVec4(255, 255, 255);
	background_primary = RgbaToImVec4(38, 38, 38);
	background_secondary = RgbaToImVec4(23, 23, 23);
	background_tertiary = RgbaToImVec4(56, 56, 56);
	highlight_primary = RgbaToImVec4(116, 77, 169);
	highlight_secondary = RgbaToImVec4(141, 124, 192);
	SetInterfaceStyle();
}

void Editor::DrawConsole(const char* window_name)
{
	ImGui::PushFont(editor_font);
	ImGui::Begin(window_name, nullptr);
	DrawActiveTitleLine(highlight_primary, background_tertiary);
	ImGui::Indent(8.0f);
	ImGui::Spacing();

	if (ImGui::Button("Clear"))
		console_capture->Clear();

	std::vector<std::string> lines = console_capture->GetLines();
	for (const std::string& line : lines)
	{
		auto [color, text] = ParseAnsiLine(line);
		//ImGui::PushTextWrapPos(0.0f);
		ImGui::TextColored(color, "%s", text.c_str());
		//ImGui::PopTextWrapPos();
	}

	ImGui::PopFont();
	ImGui::Unindent(8.0f);
	ImGui::End();
}

void Editor::SetInterfaceStyle()
{
  ImGuiStyle* style = &ImGui::GetStyle();
  ImVec4* colors = style->Colors;

  colors[ImGuiCol_Text] = text_primary;
  colors[ImGuiCol_TextDisabled] = ImVec4(text_primary.x, text_primary.y, text_primary.z, 0.5f);
  colors[ImGuiCol_WindowBg] = background_primary;
  colors[ImGuiCol_ChildBg] = background_primary;
  colors[ImGuiCol_PopupBg] = background_secondary;
  colors[ImGuiCol_Border] = background_tertiary;
  colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  colors[ImGuiCol_FrameBg] = background_secondary;
  colors[ImGuiCol_FrameBgHovered] = highlight_primary;
  colors[ImGuiCol_FrameBgActive] = highlight_secondary;
  colors[ImGuiCol_TitleBg] = background_secondary;
  colors[ImGuiCol_TitleBgActive] = background_secondary;
  colors[ImGuiCol_TitleBgCollapsed] = background_secondary;
  colors[ImGuiCol_MenuBarBg] = background_secondary;
  colors[ImGuiCol_ScrollbarBg] = background_primary;
  colors[ImGuiCol_ScrollbarGrab] = background_tertiary;
  colors[ImGuiCol_ScrollbarGrabHovered] = highlight_primary;
  colors[ImGuiCol_ScrollbarGrabActive] = highlight_secondary;
  colors[ImGuiCol_CheckMark] = highlight_primary;
  colors[ImGuiCol_SliderGrab] = highlight_primary;
  colors[ImGuiCol_SliderGrabActive] = highlight_primary;
  colors[ImGuiCol_Button] = background_tertiary;
  colors[ImGuiCol_ButtonHovered] = highlight_secondary;
  colors[ImGuiCol_ButtonActive] = highlight_primary;
  colors[ImGuiCol_Header] = background_tertiary;
  colors[ImGuiCol_HeaderHovered] = highlight_primary;
  colors[ImGuiCol_HeaderActive] = highlight_primary;
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
  colors[ImGuiCol_TabSelectedOverline] = highlight_primary;
  colors[ImGuiCol_TabDimmedSelectedOverline] = background_tertiary;
  colors[ImGuiCol_DockingPreview] = highlight_primary;
  colors[ImGuiCol_DockingEmptyBg] = background_secondary;
  colors[ImGuiCol_PlotLines] = highlight_primary;
  colors[ImGuiCol_PlotLinesHovered] = highlight_secondary;
  colors[ImGuiCol_PlotHistogram] = highlight_primary;
  colors[ImGuiCol_PlotHistogramHovered] = highlight_secondary;
  colors[ImGuiCol_TableHeaderBg] = background_secondary;
  colors[ImGuiCol_TableBorderStrong] = background_tertiary;
  colors[ImGuiCol_TableBorderLight] = background_tertiary;
  colors[ImGuiCol_TableRowBg] = background_primary;
  colors[ImGuiCol_TableRowBgAlt] = background_secondary;
  colors[ImGuiCol_TextLink] = highlight_primary;
  colors[ImGuiCol_TextSelectedBg] = highlight_primary;
  colors[ImGuiCol_TreeLines] = background_tertiary;
  colors[ImGuiCol_DragDropTarget] = highlight_secondary;
  colors[ImGuiCol_NavCursor] = highlight_primary;
  colors[ImGuiCol_NavWindowingHighlight] = highlight_primary;
  colors[ImGuiCol_CheckboxSelectedBg] = background_secondary;
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(background_primary.x, background_primary.y, background_primary.z, 0.5f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(background_primary.x, background_primary.y, background_primary.z, 0.5f);

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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
	Scene& scene = renderer.GetScene();

	ImGui::PushFont(editor_font);
    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
	        if (ImGui::MenuItem("Save All", "Ctrl+S"))
	        {
	        	SaveEditorConfig();
	            renderer.GetScene().SaveScene(renderer.GetParamDatabase());
	    		renderer.GetParamDatabase().SaveParams(renderer.GetScene().GetMaterials());
	        }
            if (ImGui::MenuItem("Build"))
            {
	            BuildProject();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) { engine.SetEditorRunState(false); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor"))
        {
	        if (ImGui::MenuItem("Save"))
	        {
	        	SaveEditorConfig();
	        }
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

	    if (ImGui::BeginMenu("View"))
	    {
	    	if (ImGui::MenuItem("Scene Editor"))
	    	{
	    		view = 0;
	    	}
	    	if (ImGui::MenuItem("Param Editor"))
	    	{
	    		view = 1;
	    	}
	    	ImGui::EndMenu();
	    }

	    if (ImGui::BeginMenu("Scene"))
	    {
            if (ImGui::MenuItem("Save"))
            {
            	if (!engine.GetProjectRunState())
					renderer.GetScene().SaveScene(renderer.GetParamDatabase());
            	else
            		Log::Warning("Project must not be running in order to save scene");
            }
	    	if (ImGui::MenuItem("Reload"))
	    	{
	    		if (!engine.GetProjectRunState())
	    			renderer.LoadScene(renderer.GetCurrentSceneIndex());
	    		else
	    			Log::Warning("Project must not be running in order to reload scene");
	    	}
            ImGui::EndMenu();
	    }

	    if (ImGui::BeginMenu("Params"))
	    {
	    	if (ImGui::MenuItem("Save"))
	    	{
	    		renderer.GetParamDatabase().SaveParams(renderer.GetScene().GetMaterials());
	    	}
	    	if (ImGui::MenuItem("Load"))
	    	{
	    		renderer.GetParamDatabase().LoadParams();
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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	PhysicsManager& physics_system = engine.GetPhysicsManager();
	Renderer& renderer = engine.GetRenderer();
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

	if (!FloatEquals(viewport_panel_size.x, editor_viewport_size.x) || !FloatEquals(viewport_panel_size.y, editor_viewport_size.y))
	{
		if (viewport_panel_size.x > 0 && viewport_panel_size.y > 0)
		{
			editor_viewport_size = {viewport_panel_size.x, viewport_panel_size.y};
			editor_viewport_framebuffer->Resize(editor_viewport_size.x, editor_viewport_size.y);
		}
	}

	ImGui::Image((void*)(intptr_t)editor_viewport_framebuffer->GetColorAttachment(), viewport_panel_size, ImVec2(0,1), ImVec2(1, 0));
    ImVec2 viewport_min = ImGui::GetItemRectMin();
    ImVec2 viewport_max = ImGui::GetItemRectMax();
    float viewport_width = viewport_max.x - viewport_min.x;
    float viewport_height = viewport_max.y - viewport_min.y;

    if (selected_entity != nullptr && gizmo_type != -1)
    {
    	ImGuizmo::SetOrthographic(engine_camera->IsOrthographic());
    	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
	Scene& scene = renderer.GetScene();

	ImGui::PushFont(editor_font);

	renderer.SetProjectViewportVisible(ImGui::Begin("Project Name Here", nullptr, window_flags));
	if (engine.GetEditorRunState())
		DrawActiveTitleLine(highlight_primary, background_tertiary);

	renderer.SetProjectViewportFocused(ImGui::IsWindowFocused());
	renderer.SetProjectViewportHovered(ImGui::IsWindowHovered());
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
	Engine& engine = Engine::Instance();
	Renderer& renderer = Engine::GetRenderer();

	ImGui::PushFont(editor_font);
    ImGui::Begin("Debug Info", nullptr);
    DrawActiveTitleLine(highlight_primary, background_tertiary);
    ImGui::Indent(8.0f);
    ImGui::Spacing();

    std::string delta_time = "Delta Time: " + std::to_string(engine.GetDeltaTime());
    std::string frame_time = "Frame Time: " + std::to_string(engine.GetDeltaTime() * 1000.0f);
    std::string frame_rate = "Frame Rate: " + std::to_string(std::lround((1.0f / engine.GetDeltaTime())));
    ImGui::Text(delta_time.c_str());
    ImGui::Text(frame_time.c_str());
    ImGui::Text(frame_rate.c_str());

	ImGui::Checkbox("Draw Colliders", &renderer.GetDrawColliders());
	ImGui::Checkbox("Draw Mesh Colliders", &renderer.GetDrawMeshColliders()); ImGui::SameLine(); ImGui::TextColored(ImColor(1.0, 1.0, 0.0, 1), "May have performance impact");
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
	Engine& engine = Engine::Instance();
    PhysicsManager& physics_system = Engine::GetPhysicsManager();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
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

    bool project_running = engine.GetProjectRunState();
    if (project_running)
    	ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    if (ImGui::ImageButton("##play_icon", (void*)(intptr_t)play_icon->gl_id, ImVec2(20, 20)))
    {
    	// play
    	if (!engine.GetProjectRunState())
    	{
    		Log::Info("Running...");
    		engine.SetProjectRunState(true);
    		selected_entity = nullptr;
    		serialized_scene_data = scene.SerializeToString(param_database);
    		Engine::GetScriptManager().ScriptsAttach();
    		ImGui::SetWindowFocus("Project Name Here");
    	}
    	// stop playing
    	else if (engine.GetProjectRunState())
    	{
    		Log::Info("Stopping...");
    		selected_entity = nullptr;
    		Engine::GetScriptManager().ScriptsDetach();
    		renderer.LoadScene(renderer.GetCurrentSceneIndex(), serialized_scene_data);
    		engine.SetProjectRunState(false);
    		serialized_scene_data.clear();
    		ImGui::SetWindowFocus("Viewport");
    	}
    }
    if (project_running)
    	ImGui::PopStyleColor(1);

    ImGui::SameLine();

    int temp_gizmo_type = gizmo_type;
    if (temp_gizmo_type == ImGuizmo::TRANSLATE)
    	ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    if (ImGui::ImageButton("##move_icon", (void*)(intptr_t)move_icon->gl_id, ImVec2(20, 20)))
    	gizmo_type = ImGuizmo::TRANSLATE;
    if (temp_gizmo_type == ImGuizmo::TRANSLATE)
    	ImGui::PopStyleColor(1);

    ImGui::SameLine();
    if (temp_gizmo_type == ImGuizmo::ROTATE)
    	ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    if (ImGui::ImageButton("##rotate_icon", (void*)(intptr_t)rotate_icon->gl_id, ImVec2(20, 20)))
    	gizmo_type = ImGuizmo::ROTATE;
    if (temp_gizmo_type == ImGuizmo::ROTATE)
    	ImGui::PopStyleColor(1);

    ImGui::SameLine();
    if (temp_gizmo_type == ImGuizmo::SCALE)
    	ImGui::PushStyleColor(ImGuiCol_Button, highlight_primary);
    if (ImGui::ImageButton("##resize_icon", (void*)(intptr_t)resize_icon->gl_id, ImVec2(20, 20)))
    	gizmo_type = ImGuizmo::SCALE;
    if (temp_gizmo_type == ImGuizmo::SCALE)
    	ImGui::PopStyleColor(1);

	ImGui::PopFont();
    ImGui::Unindent(8.0f);
    ImGui::End();
}

void Editor::DrawProjectSettings()
{
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
	Scene& scene = renderer.GetScene();

	ImGui::PushFont(editor_font);
    ImGui::Begin("Project Settings", nullptr);
    DrawActiveTitleLine(highlight_primary, background_tertiary);
    ImGui::Indent(8.0f);
    ImGui::PushTextWrapPos(0.0f);
    ImGui::Spacing();

	ImGui::Text(("Scene: " + renderer.GetScene().path).c_str());
	if (ImGui::Button(renderer.GetScene().path.c_str(), ImVec2(100, 22)))
		ImGui::OpenPopup("ChangeCurrentScene");
	ImGui::SameLine(); ImGui::Text("Current Scene");

	if (ImGui::BeginPopup("ChangeCurrentScene"))
	{
		for (int i = 0; i < renderer.GetScenes().size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::Selectable(renderer.GetScenes().at(i)->path.c_str(), false, 0))
			{
				selected_entity = nullptr;
				renderer.LoadScene(i);
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopID();
		}
		ImGui::EndPopup();
	}

	ImGui::Separator();
	ImGui::PushItemWidth(100.0f);
    ImGui::DragFloat("Drag Step", &drag_step, 0.1f, 0.1f, 100.0f, "%.2f");
	int temp_undo_redo_steps = undo_redo_steps;
	ImGui::SliderInt("Undo/Redo Steps", &temp_undo_redo_steps, 1, 256);
	undo_redo_steps = (uint8_t)temp_undo_redo_steps;

	ImGui::Separator();
	ImGui::Text("Engine Camera");
	ImGui::SliderFloat("Speed", &engine_camera_speed, 1.0f, 500.0f, "%.f");
	ImGui::SliderFloat("Sensitivity", &engine_camera_turn_sensitivity, 0.1f, 10.0f, "%.1f");
    ImGui::PopItemWidth();
	ImGui::SetNextItemWidth(300.0f);
	ImGui::DragFloat3("Position", (float*)&engine_camera->position, 1.0f, -1000.0f, 1000.0f, "%.3f");

    ImGui::Separator();
    ImGui::PushItemWidth(300.0f);
    ImGui::Text("Directional Light");
    ImGui::SliderFloat3("Direction", (float*)&scene.GetDirectionalLight()->direction, -360.0f, 360.0f, "%1.f");
	glm::vec3 color = glm::vec3(scene.GetDirectionalLight()->color.r / 255.0f, scene.GetDirectionalLight()->color.g / 255.0f, scene.GetDirectionalLight()->color.b / 255.0f);
    ImGui::ColorEdit3("Color##DL", (float*)&color);
	scene.GetDirectionalLight()->color = color * 255.0f;
    ImGui::PopItemWidth();

	ImGui::Separator();
	auto& fog = scene.GetFog();
	ImGui::Text("Fog"); ImGui::SameLine(); ImGui::Checkbox("##FogEnabled", &fog->enabled);
	if (fog->enabled)
	{
		ImGui::ColorEdit3("Color##F", (float*)&fog->color);
		ImGui::SliderFloat("Density", &fog->density, 0.001f, 1.0f, "%.3f");
		ImGui::SliderFloat("Start", &fog->start, 0.0f, 500.0f, "%.f");
		ImGui::SliderFloat("End", &fog->end, 0.0f, 500.0f, "%.f");

		const char* fog_types[] = { "Linear", "Exponential", "Exponential Squared" };
		int current_fog_type = static_cast<int>(fog->type);
		if (ImGui::Combo("Type", &current_fog_type, fog_types, 3))
			fog->type = static_cast<FogType>(current_fog_type);
	}

    ImGui::PopFont();
	ImGui::PopTextWrapPos();
    ImGui::Unindent(8.0f);
    ImGui::End();
}

void Editor::DrawHierarchy()
{
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
	Scene& scene = renderer.GetScene();

	ImGui::PushFont(editor_font);
    ImGui::Begin("Details", nullptr);
    DrawActiveTitleLine(highlight_primary, background_tertiary);
    ImGui::Indent(8.0f);
    ImGui::PushTextWrapPos(0.0f);
    ImGui::Spacing();

    PhysicsManager& physics_system = Engine::GetPhysicsManager();

    if (selected_entity == nullptr)
    {
    	ImGui::Text("No Entity Selected");
    }
    else
    {
    	ImGui::Checkbox("##Enabled", &selected_entity->enabled);
    	ImGui::SameLine();
    	ImGui::SetNextItemWidth(200.0f);
    	ImGui::InputText(" ", &selected_entity->name); ImGui::SameLine(); ImGui::TextColored(highlight_secondary, std::to_string(selected_entity->id).c_str());

    	ImGui::Separator();
    	ImGui::Text("Transform");
    	ImGui::Spacing();

    	ImGui::PushItemWidth(200.0f);

    	if (ImGui::DragFloat3("Position ", (float*)&selected_entity->position, drag_step, -1000.0f, 1000.0f, "%.3f"))
    		selected_entity->UpdateComponents();

    	glm::vec3 current_scale = selected_entity->scale;
    	if (ImGui::DragFloat3("Scale ", (float*)&current_scale, drag_step, 0.001f, 1000.0f, "%.3f"))
    	{
    		if (current_scale.x != 0.0f && current_scale.y != 0.0f && current_scale.z != 0.0f)
    		{
    			selected_entity->scale = current_scale;
    			selected_entity->UpdateComponents();
    		}
    	}

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

    	ImGui::Separator();
    	if (selected_entity->parent)
    		ImGui::Text(("Parent: " + scene.GetEntities().at(selected_entity->parent)->name).c_str());
    	if (selected_entity->children.size() > 0)
    	{
    		ImGui::Text("Children:");
    		ImGui::Indent(8.0f);
    		for (uint32_t child_id : selected_entity->children)
    			ImGui::Text(scene.GetEntities().at(child_id)->name.c_str());
    		ImGui::Unindent(8.0f);
    	}

    	ImGui::PopID();
    }

    ImGui::PopFont();
	ImGui::PopTextWrapPos();
    ImGui::Unindent(8.0f);
    ImGui::End();
}

void Editor::DrawEntityTree(std::shared_ptr<Entity> entity)
{
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
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
	Engine& engine = Engine::Instance();
	Window& project_window = engine.GetWindow();
	Renderer& renderer = engine.GetRenderer();
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
	Engine& engine = Engine::Instance();

	try
    {
        std::string project_name = engine.GetProjectConfig().project_name;
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

                std::ofstream config_out(config_path);
                if (config_out.is_open())
                {
                    config_out << std::setw(4) << config_json << "\n";
                    config_out.close();
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
