#include "bonfire_pch.hpp"
#include "Editor.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"

namespace Bonfire
{
    Editor::Editor()
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
    	
    	IMGUI_CHECKVERSION();
    	ImGui::CreateContext();
    	ImGuiIO& io = ImGui::GetIO();
    	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    	UpdateInterfaceStyle();
    	ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    	ImGui_ImplOpenGL3_Init("#version 460");
    	
    	console_capture = std::make_unique<ConsoleCapture>();
    	console_capture->StartCapture();

    	GLFWimage images[1];
    	stbi_set_flip_vertically_on_load(false);
    	int* channels = new int(4);
    	images[0].pixels = stbi_load("Data/Editor/Icons/bonfire-logo.png", &images[0].width, &images[0].height, channels, 0);
    	glfwSetWindowIcon(glfw_window, 1, images);
    	stbi_image_free(images[0].pixels);
    	
    	font_title = io.Fonts->AddFontFromFileTTF("Data/Editor/Defaults/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    	font_body = io.Fonts->AddFontFromFileTTF("Data/Editor/Defaults/Fonts/Space_Mono/SpaceMono-Regular.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    	
    	new_model_path = "Data/Editor/Defaults/Models/Cube.obj";
    	new_texture_path = "Data/Editor/Defaults/Textures/default.png";
    	new_shader_vert_path = "Data/Editor/Defaults/Shaders/unlit.vert";
    	new_shader_frag_path = "Data/Editor/Defaults/Shaders/unlit.frag";
    	new_shader_geom_path = "Data/Editor/Defaults/Shaders/unlit.geom";

    	play_icon = std::make_unique<Texture>("Data/Editor/Icons/play-icon.png", TextureType::DIFFUSE, false);
    	move_icon = std::make_unique<Texture>("Data/Editor/Icons/move-icon.png", TextureType::DIFFUSE, false);
    	rotate_icon = std::make_unique<Texture>("Data/Editor/Icons/rotate-icon.png", TextureType::DIFFUSE, false);
    	resize_icon = std::make_unique<Texture>("Data/Editor/Icons/resize-icon.png", TextureType::DIFFUSE, false);
    	play_icon->Load();
    	move_icon->Load();
    	rotate_icon->Load();
    	resize_icon->Load();

    	engine_camera = std::make_unique<Camera>(10);
    	
    	selected_entity = nullptr;
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
    	Renderer& renderer = project.GetRenderer();
    	Scene& scene = renderer.GetScene();
    	GLFWwindow* glfw_window = project_window.GetNativeWindow();
    	
    	if (editor_viewport_focused && engine_camera_can_move)
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
}
