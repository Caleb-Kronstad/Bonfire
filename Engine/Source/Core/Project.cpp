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

	Project::Project(std::string projectName)
	{
		project_name = "Bonfire: " +  projectName;
		static_project_instance = this;
		static_editor = new Editor("Data/Editor/editorconfig.bonfire");
		static_renderer = new Renderer();
		static_physics_system = new PhysicsSystem();
		static_audio_system = new AudioSystem();

		window = Window(WindowProperties(1280, 720, 0, 0, project_name));
		/*unsigned int viewportWidth = window.GetWidth() * viewportSizeAdjust;
		unsigned int viewportHeight = window.GetHeight() * viewportSizeAdjust;
		m_ViewportProps = WindowProperties(viewportWidth, viewportHeight, window.GetWidth() - viewportWidth, window.GetHeight() - viewportHeight, "Viewport");*/
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

		InitializeOpenGL();

		glfwSetKeyCallback(window.GetNativeWindow(), KeyCallbackDispatch);
		glfwSetMouseButtonCallback(window.GetNativeWindow(), MouseButtonCallbackDispatch);
		glfwSetCursorPosCallback(window.GetNativeWindow(), MouseCallbackDispatch);
		glfwSetScrollCallback(window.GetNativeWindow(), ScrollCallbackDispatch);
		glfwSetFramebufferSizeCallback(window.GetNativeWindow(), FramebufferSizeCallbackDispatch);

		static_audio_system->OnAttach();
		static_physics_system->OnAttach();
		static_renderer->OnAttach();
		static_editor->OnAttach();
		for (const auto& layer : layers)
			layer->OnAttach();

		while (running)
		{
			TickDeltaTime();

			// Update Project
			if (editor_running)
				static_editor->OnUpdate(delta_time);
			if (project_running)
			{
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
			if (editor_running)
				static_editor->OnInterfaceUpdate();
			if (project_running)
			{
				for (const auto& layer : layers)
					layer->OnInterfaceUpdate();
			}
			if (editor_running)
				static_editor->OnInterfaceEndUpdate();

			glfwSwapBuffers(window.GetNativeWindow());
			glfwPollEvents();
			
			if (glfwWindowShouldClose(window.GetNativeWindow()))
				running = false;
		}

		for (const auto& layer : layers)
			layer->OnDetach();
		static_renderer->OnDetach();
		static_audio_system->OnDetach();
		static_physics_system->OnDetach();
		static_editor->OnDetach();
		glfwDestroyWindow(window.GetNativeWindow());
		glfwTerminate();
		delete static_editor;
		delete static_audio_system;
		delete static_physics_system;
		delete static_renderer;
		delete static_project_instance;
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
		window.GetWidth() = width;
		window.GetHeight() = height;
		/*unsigned int viewportWidth = window.GetWidth() * viewportSizeAdjust;
		unsigned int viewportHeight = window.GetHeight() * viewportSizeAdjust;
		m_ViewportProps.Width = viewportWidth;
		m_ViewportProps.Height = viewportHeight;
		m_ViewportProps.xOffset = window.GetWidth() - viewportWidth;
		m_ViewportProps.yOffset = window.GetHeight() - viewportHeight;
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
		if (anti_aliasing_level > 0)
			glfwWindowHint(GLFW_SAMPLES, anti_aliasing_level);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		int user_monitor_width = mode->width;
		int user_monitor_height = mode->height;

		window.SetNativeWindow(glfwCreateWindow(window.GetWidth(), window.GetHeight(), project_name.c_str(), NULL, NULL));

		if (window.GetNativeWindow() == NULL)
		{
			Log::Error("Error creating GLFW window");
			return;
		}

		glfwMakeContextCurrent(window.GetNativeWindow());

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			Log::Error("Error Initializing GLAD");

		if (anti_aliasing_level > 0)
			glEnable(GL_MULTISAMPLE); // enable antialiasing
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glEnable(GL_STENCIL_TEST);

		glfwMaximizeWindow(window.GetNativeWindow());
		glViewport(window.GetXOffset(), window.GetYOffset(), user_monitor_width, user_monitor_height);
	}

	void Project::TickDeltaTime()
	{
		float currentFrameTime = static_cast<float>(glfwGetTime());
		delta_time = currentFrameTime - last_frame_time;
		last_frame_time = currentFrameTime;
	}
}