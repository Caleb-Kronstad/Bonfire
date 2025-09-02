#include "bonfire_pch.hpp"
#include "Project.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
	Project* Project::static_project_instance = nullptr;
	Renderer* Project::static_renderer = nullptr;
	Interface* Project::static_interface = nullptr;

	Project::Project(std::string projectName)
	{
		project_name = "Bonfire: " +  projectName;
		static_project_instance = this;
		static_renderer = new Renderer();
		static_interface = new Interface();

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

		static_interface->OnAttach();
		static_renderer->OnAttach();
		for (const auto& layer : layers)
			layer->OnAttach();

		while (engine_running)
		{
			TickDeltaTime();

			// Update Project
			static_renderer->OnUpdate();
			for (const auto& layer : layers)
				layer->OnUpdate();

			// Update Interface
			static_interface->Begin();
			static_interface->OnUpdate();
			static_renderer->OnInterfaceUpdate();
			for (const auto& layer : layers)
				layer->OnInterfaceUpdate();
			static_interface->End();

			glfwSwapBuffers(window.GetNativeWindow());
			glfwPollEvents();
			
			if (glfwWindowShouldClose(window.GetNativeWindow()))
				engine_running = false;
		}

		for (const auto& layer : layers)
			layer->OnDetach();
		static_renderer->OnDetach();
		static_interface->OnDetach();
		glfwDestroyWindow(window.GetNativeWindow());
		glfwTerminate();
		delete static_interface;
		delete static_renderer;
		delete static_project_instance;
	}

	void Project::keycallback(GLFWwindow* glfw_window, int keycode, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			KeyPressedInput input(keycode);

			static_renderer->OnInput(input);
			static_interface->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			KeyReleasedInput input(keycode);

			static_renderer->OnInput(input);
			static_interface->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
	}

	void Project::mousebuttoncallback(GLFWwindow* glfw_window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			MouseButtonPressedInput input(button);

			static_renderer->OnInput(input);
			static_interface->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			MouseButtonReleasedInput input(button);

			static_renderer->OnInput(input);
			static_interface->OnInput(input);
			for (const auto& layer : layers)
				layer->OnInput(input);
		}
	}

	void Project::mousecallback(GLFWwindow* glfw_window, double xposin, double yposin)
	{
		MouseMovedInput input(xposin, yposin);

		static_renderer->OnInput(input);
		static_interface->OnInput(input);
		for (const auto& layer : layers)
			layer->OnInput(input);
	}

	void Project::scrollcallback(GLFWwindow* glfw_window, double xoffset, double yoffset)
	{
		MouseScrolledInput input(xoffset, yoffset);

		static_renderer->OnInput(input);
		static_interface->OnInput(input);
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

		glViewport(window.GetXOffset(), window.GetYOffset(), window.GetWidth(), window.GetHeight());
	}

	void Project::TickDeltaTime()
	{
		float currentFrameTime = static_cast<float>(glfwGetTime());
		delta_time = currentFrameTime - last_frame_time;
		last_frame_time = currentFrameTime;
	}
}