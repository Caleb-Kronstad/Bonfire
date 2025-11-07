#include "bonfire_pch.hpp"
#include "Project.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
	Project* Project::static_project_instance = nullptr;
	Editor* Project::static_editor = nullptr;
	Renderer* Project::static_renderer = nullptr;
	PhysicsSystem* Project::static_physics_system = nullptr;
	AudioSystem* Project::static_audio_system = nullptr;
	ScriptSystem* Project::static_script_system = nullptr;

	Project::Project(std::string project_name)
	{
		static_project_instance = this;
		static_editor = new Editor("Data/editorconfig.bonfire");
		static_renderer = new Renderer();
		static_physics_system = new PhysicsSystem();
		static_audio_system = new AudioSystem();
		static_script_system = new ScriptSystem();

		window = std::make_unique<Window>(WindowProperties(1280, 720, 0, 0, project_name));
	}

	Project::~Project()
	{
		for (const auto& layer : layers)
		{
			layer->OnDetach();
		}
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

		editor_running = project_config.enable_editor;
		project_running = !project_config.enable_editor;

		InitializeOpenGL();

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
		std::unique_ptr<Scene> initial_scene = std::make_unique<Scene>(project_config.initial_scene_path);
		static_renderer->AddScene(std::move(initial_scene));
		static_renderer->Load();
		static_editor->OnAttach();
		
		while (running)
		{
			TickDeltaTime();

			// Update Project
			static_editor->OnUpdate(delta_time);
			if (project_running)
			{
				static_script_system->OnUpdate(delta_time);
				static_physics_system->OnUpdate(delta_time);
				static_audio_system->OnUpdate(delta_time);
			}
			static_renderer->OnUpdate(delta_time);
			if (project_running)
			{
				for (const auto& layer : layers)
					layer->OnUpdate(delta_time);
			}

			// Update Interface
			static_editor->OnInterfaceUpdate();
			if (project_running)
			{
				for (const auto& layer : layers)
					layer->OnInterfaceUpdate();
			}
			static_editor->OnInterfaceEndUpdate();

			glfwSwapBuffers(window->GetNativeWindow());
			glfwPollEvents();
			
			if (glfwWindowShouldClose(window->GetNativeWindow()))
				running = false;
		}

		static_script_system->OnDetach();
		static_renderer->OnDetach();
		static_audio_system->OnDetach();
		static_physics_system->OnDetach();
		static_editor->OnDetach();
		glfwDestroyWindow(window->GetNativeWindow());
		glfwTerminate();
		delete static_script_system;
		delete static_editor;
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
			project_config.enable_editor = json["enable-editor"].get<bool>();
			project_config.initial_scene_path = json["initial-scene-path"].get<std::string>();
			project_config.vsync = json["vsync"].get<bool>();
			project_config.project_manager_script_path = json["project-manager-script"].get<std::string>();
			project_config.antialiasing_level = json["antialiasing-level"].get<int>();
			project_config.shadow_resolution = json["shadow-resolution"].get<int>();

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

			static_editor->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			KeyReleasedInput input(keycode);

			static_editor->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
	}

	void Project::mousebuttoncallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			MouseButtonPressedInput input(button);

			static_editor->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			MouseButtonReleasedInput input(button);

			static_editor->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
	}

	void Project::mousecallback(GLFWwindow* glfw_window, double xposin, double yposin)
	{
		MouseMovedInput input(xposin, yposin);

		static_editor->OnInput(input);
		for (const auto& layer : layers)
			layer->OnInput(input);
	}

	void Project::scrollcallback(GLFWwindow* glfw_window, double xoffset, double yoffset)
	{
		MouseScrolledInput input(xoffset, yoffset);

		static_editor->OnInput(input);
		for (const auto& layer : layers)
			layer->OnInput(input);
	}

	void Project::framebuffersizecallback(GLFWwindow* glfw_window, int width, int height)
	{
		window->GetWidth() = width;
		window->GetHeight() = height;
		/*unsigned int viewportWidth = window->GetWidth() * viewportSizeAdjust;
		unsigned int viewportHeight = window->GetHeight() * viewportSizeAdjust;
		m_ViewportProps.Width = viewportWidth;
		m_ViewportProps.Height = viewportHeight;
		m_ViewportProps.xOffset = window->GetWidth() - viewportWidth;
		m_ViewportProps.yOffset = window->GetHeight() - viewportHeight;
		glViewport(m_ViewportProps.xOffset, m_ViewportProps.yOffset, m_ViewportProps.Width, m_ViewportProps.Height);*/

		glViewport(0, 0, width, height); // for testing before adding in custom rendering window size
	}

	void Project::InitializeOpenGL()
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
}