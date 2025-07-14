#pragma once

#include "Input/Input.hpp"
#include "Input/InputCodes.hpp"
#include "Input/InputTypes.hpp"

#include "Interface/Interface.hpp"

#include "Core/Window.hpp"
#include "Core/Layer.hpp"

#include "Renderer/Renderer.hpp"

namespace Bonfire
{
	class Project
	{
	public:
		Project(std::string projectName = "New Project");
		virtual ~Project();

		void Run();
		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		// Getters
		static Project& GetInstance() { return *s_Instance; }
		static Renderer& GetRenderer() { return *s_Renderer; }
		bool& GetProjectRunState() { return m_ProjectRunning; }
		bool& GetEngineRunState() { return m_EngineRunning; }
		std::string GetProjectName() const { return m_ProjectName; }
		float GetDeltaTime() const { return m_DeltaTime; }
		Window& GetWindow() { return m_Window; }

		// Callback functions
		void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mousebuttoncallback(GLFWwindow* window, int button, int action, int mods);
		void mousecallback(GLFWwindow* window, double xposin, double yposin);
		void scrollcallback(GLFWwindow* window, double xoffset, double yoffset);
		void framebuffersizecallback(GLFWwindow* window, int width, int height);

		static void keycallback_dispatch(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (s_Instance)
				s_Instance->keycallback(window, key, scancode, action, mods);
		}
		static void mousebuttoncallback_dispatch(GLFWwindow* window, int key, int action, int mods)
		{
			if (s_Instance)
				s_Instance->mousebuttoncallback(window, key, action, mods);
		}
		static void mousecallback_dispatch(GLFWwindow* window, double xposin, double yposin)
		{
			if (s_Instance)
				s_Instance->mousecallback(window, xposin, yposin);
		}
		static void scrollcallback_dispatch(GLFWwindow* window, double xoffset, double yoffset)
		{
			if (s_Instance)
				s_Instance->scrollcallback(window, xoffset, yoffset);
		}
		static void framebuffersizecallback_dispatch(GLFWwindow* window, int width, int height)
		{
			if (s_Instance)
				s_Instance->framebuffersizecallback(window, width, height);
		}

	private:
		void InitializeOpenGL();
		void TickDeltaTime();

	private:
		float m_DeltaTime = 0.0f;
		float m_LastFrameTime = 0.0f;

	private:
		Window m_Window;
		/*WindowProperties m_ViewportProps;
		float viewportSizeAdjust = 1;*/
		
		unsigned int m_AntiAliasingLevel = 4;
		bool m_EngineRunning = true;
		bool m_ProjectRunning = false;

		std::string m_ProjectName;
		
		std::vector<Layer*> m_Layers;

		static Renderer* s_Renderer;
		static Interface* s_Interface;
		static Project* s_Instance;
	};
}