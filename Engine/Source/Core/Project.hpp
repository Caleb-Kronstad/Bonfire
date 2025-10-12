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
		void PushLayer(std::shared_ptr<Layer> layer);
		void PopLayer(std::shared_ptr<Layer> layer);

		// Getters
		static Project& GetInstance() { return *static_project_instance; }
		static Renderer& GetRenderer() { return *static_renderer; }
		static Interface& GetInterface() { return *static_interface; }
		const bool& GetProjectRunState() const { return project_running; }
		const bool& GetEngineRunState() const { return engine_running; }
		const std::string& GetProjectName() const { return project_name; }
		const float& GetDeltaTime() const { return delta_time; }
		const unsigned int& GetFrameCount() const { return frame_count; }
		Window& GetWindow() { return window; }

		void SetProjectRunState(bool state) { project_running = state; }
		void SetEngineRunState(bool state) { engine_running = state; }

		// Callback functions
		void keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mousebuttoncallback(GLFWwindow* window, int button, int action, int mods);
		void mousecallback(GLFWwindow* window, double xposin, double yposin);
		void scrollcallback(GLFWwindow* window, double xoffset, double yoffset);
		void framebuffersizecallback(GLFWwindow* window, int width, int height);

		static void KeyCallbackDispatch(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (static_project_instance)
				static_project_instance->keycallback(window, key, scancode, action, mods);
		}
		static void MouseButtonCallbackDispatch(GLFWwindow* window, int key, int action, int mods)
		{
			if (static_project_instance)
				static_project_instance->mousebuttoncallback(window, key, action, mods);
		}
		static void MouseCallbackDispatch(GLFWwindow* window, double xposin, double yposin)
		{
			if (static_project_instance)
				static_project_instance->mousecallback(window, xposin, yposin);
		}
		static void ScrollCallbackDispatch(GLFWwindow* window, double xoffset, double yoffset)
		{
			if (static_project_instance)
				static_project_instance->scrollcallback(window, xoffset, yoffset);
		}
		static void FramebufferSizeCallbackDispatch(GLFWwindow* window, int width, int height)
		{
			if (static_project_instance)
				static_project_instance->framebuffersizecallback(window, width, height);
		}

	private:
		void InitializeOpenGL();
		void TickDeltaTime();
		void IncrementFrameCount();

	private:
		float delta_time = 0.0f;
		float last_frame_time = 0.0f;
		unsigned int frame_count = 0;

	private:
		Window window;
		/*WindowProperties m_ViewportProps;
		float viewportSizeAdjust = 1;*/
		
		unsigned int anti_aliasing_level = 4;
		bool engine_running = true;
		bool project_running = false;

		std::string project_name;
		
		std::vector<std::shared_ptr<Layer>> layers;

		static Renderer* static_renderer;
		static Interface* static_interface;
		static Project* static_project_instance;
	};
}