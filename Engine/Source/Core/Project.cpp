#include "bonfire_pch.hpp"
#include "Project.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
	Project* Project::static_project_instance = nullptr;
	Renderer* Project::static_renderer = nullptr;
	PhysicsSystem* Project::static_physics_system = nullptr;
	AudioSystem* Project::static_audio_system = nullptr;
	ScriptSystem* Project::static_script_system = nullptr;

	Project::Project(std::string project_name)
	{
		static_project_instance = this;
		static_renderer = new Renderer();
		static_physics_system = new PhysicsSystem();
		static_audio_system = new AudioSystem();
		static_script_system = new ScriptSystem();

		window = std::make_unique<Window>(WindowProperties(1280, 720, 0, 0, project_name));
	}

	Project::~Project()
	{
	}

	void Project::PushLayer(std::shared_ptr<Layer> layer)
	{
		layers.emplace_back(layer);
	}
	void Project::PopLayer(std::shared_ptr<Layer> layer)
	{
		auto it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end())
		{
			layer->OnDetach();
			layers.erase(it);
		}
	}

	void Project::Run()
	{
		std::cout << "Current Project Path: " << std::filesystem::current_path() << "\n";

		LoadProjectConfig("Data/projectconfig.bonfire");

		window->window_props = WindowProperties(project_config.window_width, project_config.window_height, 0, 0, project_config.project_name);
		window->SetVSync(project_config.vsync);

		InitOpenGL();

		glfwSetKeyCallback(window->GetNativeWindow(), KeyCallbackDispatch);
		glfwSetMouseButtonCallback(window->GetNativeWindow(), MouseButtonCallbackDispatch);
		glfwSetCursorPosCallback(window->GetNativeWindow(), MouseCallbackDispatch);
		glfwSetScrollCallback(window->GetNativeWindow(), ScrollCallbackDispatch);
		glfwSetFramebufferSizeCallback(window->GetNativeWindow(), FramebufferSizeCallbackDispatch);

		static_audio_system->OnAttach();
		static_physics_system->OnAttach();
		
		static_script_system->OnAttach();
		static_script_system->ExecuteGlobalScript(project_config.project_manager_script_path);
		
		static_renderer->OnAttach();
		for (const std::string& scene_path : project_config.scene_paths)
		{
			std::unique_ptr<Scene> scene = std::make_unique<Scene>(scene_path);
			static_renderer->AddScene(std::move(scene));
		}
		static_renderer->GetParamDatabase().LoadParams();
		static_renderer->LoadScene(0);

		InitImGui();
		for (std::shared_ptr<Layer>& layer : layers)
			layer->OnAttach();
		
		while (running)
		{
			TickDeltaTime();

			// Update Project
			if (project_running && static_renderer->GetScene().loaded)
			{
				static_script_system->OnUpdate(delta_time);
				static_physics_system->OnUpdate(delta_time);
				static_audio_system->OnUpdate(delta_time);
			}
			
			static_renderer->OnUpdate(delta_time);
			for (const auto& layer : layers)
				layer->OnUpdate(delta_time);

			// Update Interface
			BeginImGuiFrame();
			static_renderer->OnInterfaceUpdate();
			for (const auto& layer : layers)
				layer->OnInterfaceUpdate();
			static_script_system->OnInterfaceUpdate();
			EndImGuiFrame();

			glfwSwapBuffers(window->GetNativeWindow());
			glfwPollEvents();
			
			if (glfwWindowShouldClose(window->GetNativeWindow()))
				running = false;
		}

		static_script_system->OnDetach();
		static_renderer->OnDetach();
		static_audio_system->OnDetach();
		static_physics_system->OnDetach();
		for (std::shared_ptr<Layer>& layer : layers)
			layer->OnDetach();
		glfwDestroyWindow(window->GetNativeWindow());
		glfwTerminate();
		delete static_script_system;
		delete static_audio_system;
		delete static_physics_system;
		delete static_renderer;
		delete static_project_instance;
	}

	bool Project::LoadProjectConfig(const std::string& config_path)
	{
		std::ifstream file(config_path);
		if (!file.is_open())
		{
			Log::Warning("[Project] No projectconfig.bonfire found, using defaults");
			return false;
		}

		try
		{
			nlohmann::json json;
			file >> json;

			project_config.project_name = json["project-name"].get<std::string>();
			project_config.window_width = json["window-width"].get<int>();
			project_config.window_height = json["window-height"].get<int>();
			project_config.fullscreen = json["fullscreen"].get<bool>();
			project_config.scene_paths = json["scene-paths"].get<std::vector<std::string>>();
			project_config.vsync = json["vsync"].get<bool>();
			project_config.project_manager_script_path = json["project-manager-script"].get<std::string>();
			project_config.antialiasing_level = json["antialiasing-level"].get<int>();
			project_config.shadow_resolution = json["shadow-resolution"].get<int>();

			if (project_config.scene_paths.empty())
			{
				Log::Warning("[Project] No scene path found, using defaults");
				std::filesystem::copy_file("Data/Editor/Defaults/defaultscene.bonfire", "Data/Scenes/defaultscene.bonfire");
				project_config.scene_paths.emplace_back("Data/Scenes/defaultscene.bonfire");
			}

			Log::Info("[Project] Loaded project config: " + project_config.project_name);
			return true;
		}
		catch (const std::exception& e)
		{
			Log::Error("[Project] Failed to parse projectconfig.bonfire " + std::string(e.what()));
			return false;
		}
	}


	void Project::keycallback(GLFWwindow* glfw_window, int keycode, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			KeyPressedInput input(keycode);

			for (const auto& layer : layers)
				layer->OnInput(input);
			static_script_system->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			KeyReleasedInput input(keycode);

			for (const auto& layer : layers)
				layer->OnInput(input);
			static_script_system->OnInput(input);
		}
	}

	void Project::mousebuttoncallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			MouseButtonPressedInput input(button);

			for (const auto& layer : layers)
				layer->OnInput(input);
			static_script_system->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			MouseButtonReleasedInput input(button);

			for (const auto& layer : layers)
				layer->OnInput(input);
			static_script_system->OnInput(input);
		}
	}

	void Project::mousecallback(GLFWwindow* glfw_window, double xposin, double yposin)
	{
		MouseMovedInput input(xposin, yposin);

		for (const auto& layer : layers)
			layer->OnInput(input);
		static_script_system->OnInput(input);
	}

	void Project::scrollcallback(GLFWwindow* glfw_window, double xoffset, double yoffset)
	{
		MouseScrolledInput input(xoffset, yoffset);

		for (const auto& layer : layers)
			layer->OnInput(input);
		static_script_system->OnInput(input);
	}

	void Project::framebuffersizecallback(GLFWwindow* glfw_window, int width, int height)
	{
		window->GetWidth() = width;
		window->GetHeight() = height;
		glViewport(0, 0, width, height);
	}

	void Project::InitOpenGL()
	{
		if (!glfwInit())
		{
			Log::Error("Error initializing GLFW");
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		if (project_config.antialiasing_level > 0)
			glfwWindowHint(GLFW_SAMPLES, project_config.antialiasing_level);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		int user_monitor_width = mode->width;
		int user_monitor_height = mode->height;

		if (project_config.fullscreen)
			window->SetNativeWindow(glfwCreateWindow(window->GetWidth(), window->GetHeight(), project_config.project_name.c_str(), monitor, NULL));
		else
			window->SetNativeWindow(glfwCreateWindow(window->GetWidth(), window->GetHeight(), project_config.project_name.c_str(), NULL, NULL));

		if (window->GetNativeWindow() == NULL)
		{
			Log::Error("Error creating GLFW window");
			return;
		}

		glfwMakeContextCurrent(window->GetNativeWindow());

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			Log::Error("Error Initializing GLAD");

		if (project_config.antialiasing_level > 0)
			glEnable(GL_MULTISAMPLE); // enable antialiasing
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glEnable(GL_STENCIL_TEST);

		if (!project_config.fullscreen)
		{
			glfwMaximizeWindow(window->GetNativeWindow());
			glViewport(window->GetXOffset(), window->GetYOffset(), user_monitor_width, user_monitor_height);
		}
		else
		{
			glViewport(window->GetXOffset(), window->GetYOffset(), window->GetWidth(), window->GetHeight());
		}
	}

	void Project::TickDeltaTime()
	{
		float currentFrameTime = static_cast<float>(glfwGetTime());
		delta_time = currentFrameTime - last_frame_time;
		last_frame_time = currentFrameTime;
	}

	void Project::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		
		ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void Project::BeginImGuiFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void Project::EndImGuiFrame()
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
}