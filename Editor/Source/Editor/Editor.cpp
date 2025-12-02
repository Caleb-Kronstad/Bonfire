#include "Editor.hpp"

Editor::Editor(const std::string& config_path)
    : config_path(config_path)
{
	project_path = std::filesystem::current_path().generic_string();
}
Editor::~Editor()
{
	console_capture->StopCapture();
}

void Editor::OnAttach()
{
    Project& project = Project::GetInstance();
    Window& project_window = project.GetWindow();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    GLFWwindow* glfw_window = project_window.GetNativeWindow();
	
    UpdateEditorInterfaceStyle();
    
    console_capture = std::make_unique<ConsoleCapture>();
    console_capture->StartCapture();

    GLFWimage images[1];
    stbi_set_flip_vertically_on_load(false);
    int* channels = new int(4);
    images[0].pixels = stbi_load("Data/Editor/Icons/bonfire-logo.png", &images[0].width, &images[0].height, channels, 0);
    glfwSetWindowIcon(glfw_window, 1, images);
    stbi_image_free(images[0].pixels);

	ImGuiIO& io = ImGui::GetIO();
    editor_font = io.Fonts->AddFontFromFileTTF("Data/Editor/Defaults/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    
    default_model_path = "Data/Editor/Defaults/Models/Cube.obj";
    default_diffuse_path = "Data/Editor/Defaults/Textures/default-diffuse.png";
    default_specular_path = "Data/Editor/Defaults/Textures/default-specular.png";
    default_normal_path = "Data/Editor/Defaults/Textures/default-normal.png";
    default_height_path = "Data/Editor/Defaults/Textures/default-height.png";
    default_emission_path = "Data/Editor/Defaults/Textures/default-emission.png";
    default_shader_vert_path = "Data/Editor/Defaults/Shaders/unlit.vert";
    default_shader_frag_path = "Data/Editor/Defaults/Shaders/unlit.frag";
    default_shader_geom_path = "Data/Editor/Defaults/Shaders/unlit.geom";

    auto& textures = scene.GetTextures();
    default_textures.at(0) = textures.at(1000);
    default_textures.at(1) = textures.at(1001);
    default_textures.at(2) = textures.at(1002);
    default_textures.at(3) = textures.at(1003);
    default_textures.at(4) = textures.at(1004);

    play_icon = std::make_unique<Texture>("Data/Editor/Icons/play-icon.png", TextureType::DIFFUSE, false);
    move_icon = std::make_unique<Texture>("Data/Editor/Icons/move-icon.png", TextureType::DIFFUSE, false);
    rotate_icon = std::make_unique<Texture>("Data/Editor/Icons/rotate-icon.png", TextureType::DIFFUSE, false);
    resize_icon = std::make_unique<Texture>("Data/Editor/Icons/resize-icon.png", TextureType::DIFFUSE, false);
    play_icon->Load();
    move_icon->Load();
    rotate_icon->Load();
    resize_icon->Load();

    selected_entity = nullptr;

    LoadEditorConfig();
	command_history = std::make_unique<CommandHistory>(undo_redo_steps); // we need to load editor config first to make sure we have the correct undo/redo steps value :)

	editor_viewport_framebuffer = std::make_unique<Framebuffer>(editor_viewport_size.x, editor_viewport_size.y);
    
    for (auto& [entity_id, entity] : scene.GetEntities())
    {
    	if (entity->HasComponent<ModelComponent>() && entity->HasComponent<PhysicsComponent>())
    	{
    		ModelComponent& model_component = entity->GetComponent<ModelComponent>();
    		PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
    		if (physics_component.physics_body->GetShapeData().type == PhysicsShapeType::MESH)
    		{
    			physics_component.physics_body->SetScale(entity->scale, model_component.model);
    		}
    	}
    }
}
void Editor::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::OnUpdate(const float& delta_time)
{
    Project& project = Project::GetInstance();
    Window& project_window = project.GetWindow();
    Renderer& renderer = Project::GetRenderer();
    GLFWwindow* glfw_window = project_window.GetNativeWindow();
    
    if (editor_viewport_focused && engine_camera_can_move && !renderer.GetProjectViewportFocused())
    {
    	float velocity = engine_camera_speed * delta_time;
    	if (glfwGetKey(glfw_window, InputCode::W) == GLFW_PRESS)
    		engine_camera->position += engine_camera->GetFrontVector() * velocity;
    	if (glfwGetKey(glfw_window, InputCode::S) == GLFW_PRESS)
    		engine_camera->position -= engine_camera->GetFrontVector() * velocity;
    	if (glfwGetKey(glfw_window, InputCode::A) == GLFW_PRESS)
    		engine_camera->position -= engine_camera->GetRightVector() * velocity;
    	if (glfwGetKey(glfw_window, InputCode::D) == GLFW_PRESS)
    		engine_camera->position += engine_camera->GetRightVector() * velocity;
    }

	if (EditorViewportVisible())
		renderer.RenderViewport(delta_time, *engine_camera, *editor_viewport_framebuffer, editor_viewport_size);

    RemoveItems();
}

void Editor::OnInput(Input& input)
{
    Project& project = Project::GetInstance();
    Window& project_window = project.GetWindow();
    Renderer& renderer = project.GetRenderer();
    Scene& scene = renderer.GetScene();
    GLFWwindow* glfw_window = project_window.GetNativeWindow();
    
    if (block_interface_inputs)
    {
    	ImGuiIO& io = ImGui::GetIO();
    	input.Handled |= io.WantCaptureMouse;
    	input.Handled |= io.WantCaptureKeyboard;
    }

    switch (input.GetInputType())
	{
	case InputType::KeyPressed:
		{
			const auto key_input = dynamic_cast<KeyPressedInput&>(input);

			// -- actions here --
			if (key_input.GetKeyCode() == InputCode::LeftControl)
			{
				engine_camera_can_move = false;
				CTRL_DOWN = true;
			}

			if (key_input.GetKeyCode() == InputCode::D)
			{
				if (CTRL_DOWN && selected_entity != nullptr && editor_viewport_focused)
					DuplicateEntity(selected_entity);
			}

			if (key_input.GetKeyCode() == InputCode::F5)
			{
				// play
				if (!project.GetProjectRunState())
				{
					Log::Info("Running...");
					serialized_scene_data = scene.SerializeToString(renderer.GetParamDatabase());
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
					Project::GetScriptSystem().DestroyScripts(scene);
					scene.DeserializeFromString(serialized_scene_data, renderer.GetParamDatabase());
					selected_entity = nullptr;
					ImGui::SetWindowFocus("Viewport");
				}
			}
			
			break;
		}
	case InputType::KeyReleased:
		{
			const auto key_input = dynamic_cast<KeyReleasedInput&>(input);

			// -- actions here --
			if (key_input.GetKeyCode() == InputCode::LeftControl)
			{
				engine_camera_can_move = true;
				CTRL_DOWN = false;
			}
			if (key_input.GetKeyCode() == InputCode::Z && CTRL_DOWN)
			{
				if (command_history->CanUndo())
					command_history->Undo();
			}if (key_input.GetKeyCode() == InputCode::Y && CTRL_DOWN)
			{
				if (command_history->CanRedo())
					command_history->Redo();
			}
			
			break;
		}
	case InputType::KeyTyped:
		{
			const auto key_input = dynamic_cast<KeyTypedInput&>(input);

			// -- actions here --

			break;
		}
	case InputType::MouseButtonPressed:
		{
			const auto mouse_input = dynamic_cast<MouseButtonPressedInput&>(input);
			
			// -- actions here --

			// enable engine camera rotation
			if (mouse_input.GetMouseButton() == InputCode::Button1)
			{
				if (editor_viewport_focused)
				{
					engine_camera_can_turn = true;
					engine_camera->IsFirstMouse() = true;
				}
			}
			
			break;
		}
	case InputType::MouseButtonReleased:
		{
			const auto mouse_input = dynamic_cast<MouseButtonReleasedInput&>(input);
			
			// -- actions here --

			// disable engine camera rotation
			if (mouse_input.GetMouseButton() == InputCode::Button1)
			{
				engine_camera_can_turn = false;
			}
			
			break;
		}
	case InputType::MouseMoved:
		{
			const auto mouse_input = dynamic_cast<MouseMovedInput&>(input);

			// -- actions here --
			const float x_position = mouse_input.GetX();
			const float y_position = mouse_input.GetY();

			if (engine_camera->IsFirstMouse())
			{
				engine_camera->GetLastX() = x_position;
				engine_camera->GetLastY() = y_position;
				engine_camera->IsFirstMouse() = false;
			}

			float x_offset = x_position - engine_camera->GetLastX();
			float y_offset = y_position - engine_camera->GetLastY();

			engine_camera->GetLastX() = x_position;
			engine_camera->GetLastY() = y_position;
    
			if (!engine_camera_can_turn) break;
			x_offset *= engine_camera_turn_sensitivity;
			y_offset *= engine_camera_turn_sensitivity;

			engine_camera->yaw += x_offset;
			engine_camera->pitch -= y_offset;

			if (engine_camera_constrain_pitch)
			{
				engine_camera->pitch = (std::max)(engine_camera->pitch, -89.0f);
				engine_camera->pitch = (std::min)(engine_camera->pitch, 89.0f);
			}
			
			engine_camera->UpdateCameraVectors();
			
			break;
		}
	case InputType::MouseScrolled:
		{
			const auto mouse_input = dynamic_cast<MouseScrolledInput&>(input);

			// -- actions here --
			if (editor_viewport_hovered)
				engine_camera->position += engine_camera->GetFrontVector() * mouse_input.GetYOffset();
			
			break;
		}
	case InputType::None:
			break;
	}
}

void Editor::ExecuteCommand(std::unique_ptr<Command> command)
{
	command_history->ExecuteCommand(std::move(command));
}

bool Editor::LoadEditorConfig()
{
    std::ifstream file(config_path);
    if (!file.is_open())
    {
    	Log::Error("Failed to open editor config file: " + config_path);
    	return false;
    }

    nlohmann::json json;
    try
    {
    	file >> json;
    }
    catch (const nlohmann::json::exception& e)
    {
    	Log::Error("Failed to parse editor config JSON: " + std::string(e.what()));
    	return false;
    }

    if (!json.contains("DATA-TYPE"))
    {
    	Log::Error("Unknown data type when trying to load .bonfire scene file at " + config_path);
    	return false;
    }
    std::string data_type = json["DATA-TYPE"]["type"].get<std::string>();
    if (data_type != "EDITOR CONFIG")
    {
    	Log::Error("Found incorrect data type associated with .bonfire editor config file at " + config_path + " ... " + data_type + " data type found");
    	return false;
    }

    if (!json.contains("camera"))
    {
    	engine_camera = std::make_unique<Camera>(1);
    	Log::Warning("Editor config could not find camera -- Setting to default");
    }
    nlohmann::json camera_json = json["camera"];
    float yaw = camera_json["yaw"].get<float>();
    float pitch = camera_json["pitch"].get<float>();
    auto position_array = camera_json["position"].get<std::vector<float>>();
    auto up_array = camera_json["up"].get<std::vector<float>>();
    glm::vec3 position(position_array[0], position_array[1], position_array[2]);
    glm::vec3 up(up_array[0], up_array[1], up_array[2]);
    float speed = camera_json["speed"].get<float>();
    float sensitivity = camera_json["sensitivity"].get<float>();

    engine_camera_speed = speed;
    engine_camera_turn_sensitivity = sensitivity;
    engine_camera = std::make_unique<Camera>(1, position, up, yaw, pitch);

    if (!json.contains("editor-settings"))
    {
    	drag_step = 1.0f;
    	undo_redo_steps = 64;
    	Log::Warning("Editor config could not find editor settings -- Setting to default");
    }
    nlohmann::json editor_settings_json = json["editor-settings"];
    drag_step = editor_settings_json["drag-step"].get<float>();
    undo_redo_steps = editor_settings_json["undo-redo-steps"].get<uint8_t>();

    Log::Info("Editor config loaded successfully");
    return true;
}

bool Editor::SaveEditorConfig()
{
    nlohmann::json json;

    nlohmann::json camera_json;
    camera_json["yaw"] = engine_camera->yaw;
    camera_json["pitch"] = engine_camera->pitch;
    camera_json["position"] = {engine_camera->position.x, engine_camera->position.y, engine_camera->position.z};
    camera_json["up"] = {engine_camera->GetWorldUpVector().x, engine_camera->GetWorldUpVector().y, engine_camera->GetWorldUpVector().z};
    camera_json["speed"] = engine_camera_speed;
    camera_json["sensitivity"] = engine_camera_turn_sensitivity;

    nlohmann::json settings_json;
    settings_json["undo-redo-steps"] = undo_redo_steps;
    settings_json["drag-step"] = drag_step;

    json["DATA-TYPE"]["type"] = "EDITOR CONFIG";
    json["camera"] = camera_json;
    json["editor-settings"] = settings_json;

    std::ofstream file(config_path);
    if (!file.is_open())
    {
    	Log::Error("Failed to open editor config file for writing: " + config_path);
    	return false;
    }

    try
    {
    	file << json.dump(4);
    }
    catch (const nlohmann::json::exception& e)
    {
    	Log::Error("Failed to write editor config JSON: " + std::string(e.what()));
    	return false;
    }

    Log::Info("Saved editor config to " + config_path);
    return true;
}