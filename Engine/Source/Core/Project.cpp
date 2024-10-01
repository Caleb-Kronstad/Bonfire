#include "bonfire_pch.h"
#include "Project.h"

#include "Core/Utility.h"
#include "Core/Time.h"

namespace Bonfire
{
	Project* Project::s_Instance = nullptr;
	Renderer* Project::s_Renderer = nullptr;
	EngineInterface* Project::s_EngineInterface = nullptr;

	Project::Project(std::string projectName)
		: m_ProjectName(projectName)
	{
		s_Instance = this;
		s_Renderer = new Renderer();
		s_EngineInterface = new EngineInterface();

		m_Window = Window(WindowProperties(1280, 720, 0, 0, m_ProjectName));
		float viewportWidth = m_Window.GetWidth() * viewportSizeAdjust;
		float viewportHeight = m_Window.GetHeight() * viewportSizeAdjust;
		m_ViewportProps = WindowProperties(viewportWidth, viewportHeight, m_Window.GetWidth() - viewportWidth, m_Window.GetHeight() - viewportHeight, "Viewport");
	}

	Project::~Project()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void Project::PushLayer(Layer* layer)
	{
		m_Layers.emplace_back(layer);
	}
	void Project::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			layer->OnDetach();
			m_Layers.erase(it);
		}
	}

	void Project::Run()
	{
		std::cout << "Current Project Path: " << std::filesystem::current_path() << std::endl;

		InitializeOpenGL();

		glfwSetKeyCallback(m_Window.GetNativeWindow(), keycallback_dispatch);
		glfwSetMouseButtonCallback(m_Window.GetNativeWindow(), mousebuttoncallback_dispatch);
		glfwSetCursorPosCallback(m_Window.GetNativeWindow(), mousecallback_dispatch);
		glfwSetScrollCallback(m_Window.GetNativeWindow(), scrollcallback_dispatch);
		glfwSetFramebufferSizeCallback(m_Window.GetNativeWindow(), framebuffersizecallback_dispatch);

		s_EngineInterface->OnAttach();
		s_Renderer->OnAttach();
		for (Layer* layer : m_Layers)
			layer->OnAttach();

		while (m_EngineRunning)
		{
			Time::TickDeltaTime();

			s_Renderer->OnUpdate();
			for (Layer* layer : m_Layers)
				layer->OnUpdate();
			s_EngineInterface->OnUpdate();

			glfwSwapBuffers(m_Window.GetNativeWindow());
			glfwPollEvents();

			if (glfwWindowShouldClose(m_Window.GetNativeWindow()))
				m_EngineRunning = false;
		}

		for (Layer* layer : m_Layers)
			layer->OnDetach();
		s_Renderer->OnDetach();
		s_EngineInterface->OnDetach();
		glfwTerminate();
	}

	void Project::keycallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			KeyPressedInput input(keycode);

			s_Renderer->OnInput(input);
			s_EngineInterface->OnInput(input);
			for (Layer* layer : m_Layers)
				layer->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			KeyReleasedInput input(keycode);

			s_Renderer->OnInput(input);
			s_EngineInterface->OnInput(input);
			for (Layer* layer : m_Layers)
				layer->OnInput(input);
		}
	}

	void Project::mousebuttoncallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			MouseButtonPressedInput input(button);

			s_Renderer->OnInput(input);
			s_EngineInterface->OnInput(input);
			for (Layer* layer : m_Layers)
				layer->OnInput(input);
		}
		else if (action == GLFW_RELEASE)
		{
			MouseButtonReleasedInput input(button);

			s_Renderer->OnInput(input);
			s_EngineInterface->OnInput(input);
			for (Layer* layer : m_Layers)
				layer->OnInput(input);
		}
	}

	void Project::mousecallback(GLFWwindow* window, double xposin, double yposin)
	{
		MouseMovedInput input(xposin, yposin);

		s_Renderer->OnInput(input);
		s_EngineInterface->OnInput(input);
		for (Layer* layer : m_Layers)
			layer->OnInput(input);
	}

	void Project::scrollcallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		MouseScrolledInput input(xoffset, yoffset);

		s_Renderer->OnInput(input);
		s_EngineInterface->OnInput(input);
		for (Layer* layer : m_Layers)
			layer->OnInput(input);
	}

	void Project::framebuffersizecallback(GLFWwindow* window, int width, int height)
	{
		m_Window.GetWidth() = width;
		m_Window.GetHeight() = height;
		float viewportWidth = m_Window.GetWidth() * viewportSizeAdjust;
		float viewportHeight = m_Window.GetHeight() * viewportSizeAdjust;
		m_ViewportProps.Width = viewportWidth;
		m_ViewportProps.Height = viewportHeight;
		m_ViewportProps.xOffset = m_Window.GetWidth() - viewportWidth;
		m_ViewportProps.yOffset = m_Window.GetHeight() - viewportHeight;
		glViewport(m_ViewportProps.xOffset, m_ViewportProps.yOffset, m_ViewportProps.Width, m_ViewportProps.Height);
	}

	void Project::InitializeOpenGL()
	{
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		if (m_AntiAliasingLevel > 0)
			glfwWindowHint(GLFW_SAMPLES, m_AntiAliasingLevel);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		m_Window.SetNativeWindow(glfwCreateWindow(m_Window.GetWidth(), m_Window.GetHeight(), m_ProjectName.c_str(), NULL, NULL));

		if (m_Window.GetNativeWindow() == NULL)
		{
			Log::Error("Error creating GLFW window");
			glfwTerminate();
		}

		glfwMakeContextCurrent(m_Window.GetNativeWindow());

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			Log::Error("Error Initializing GLAD");

		if (m_AntiAliasingLevel > 0)
			glEnable(GL_MULTISAMPLE); // enable antialiasing
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glEnable(GL_STENCIL_TEST);

		glViewport(m_Window.GetXOffset(), m_Window.GetYOffset(), m_Window.GetWidth(), m_Window.GetHeight());
	}
}