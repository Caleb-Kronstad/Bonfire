#pragma once

#include "Input/Input.hpp"
#include "Input/InputCodes.hpp"
#include "Input/InputTypes.hpp"

#include "Core/Window.hpp"
#include "Core/Layer.hpp"

#include "Renderer/Renderer.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Audio/AudioSystem.hpp"
#include "Scripting/ScriptSystem.hpp"

namespace Bonfire
{
	struct ProjectConfig
	{
		std::string project_name = "Bonfire Engine";
		int window_width = 1280;
		int window_height = 720;
		int shadow_resolution = 2048;
		int antialiasing_level = 4;
		bool fullscreen = true;
		std::vector<std::string> scene_paths;
		std::string project_manager_script_path;
		bool vsync = true;
	};
	
	class Project
	{
	public:
		Project(std::string project_name = "New Project");
		virtual ~Project();

		void Run();
		void PushLayer(std::shared_ptr<Layer> layer);
		void PopLayer(std::shared_ptr<Layer> layer);

		// Getters
		static Project& GetInstance() { return *static_project_instance; }
		static Renderer& GetRenderer() { return *static_renderer; }
		static PhysicsSystem& GetPhysicsSystem() { return *static_physics_system; }
		static AudioSystem& GetAudioSystem() { return *static_audio_system; }
		static ScriptSystem& GetScriptSystem() { return *static_script_system; }
		const bool& GetProjectRunState() const { return project_running; }
		const bool& GetEditorRunState() const { return editor_running; }
		const std::string& GetProjectName() const { return project_config.project_name; }
		const float& GetDeltaTime() const { return delta_time; }
		Window& GetWindow() const { return *window; }
		std::vector<std::shared_ptr<Layer>>& GetLayers() { return layers; }

		void SetProjectRunState(bool state) { project_running = state; }
		void SetEditorRunState(bool state) { editor_running = state; }

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

		const ProjectConfig& GetProjectConfig() const { return project_config; }

	private:
		void InitOpenGL();
		void TickDeltaTime();

		void InitImGui();
		void BeginImGuiFrame();
		void EndImGuiFrame();

		bool LoadProjectConfig(const std::string& config_path);

	private:
		float delta_time = 0.0f;
		float last_frame_time = 0.0f;

	private:
		ProjectConfig project_config;
		std::unique_ptr<Window> window;
		
		bool running = true;
		bool editor_running = true;
		bool project_running = false;
		std::vector<std::shared_ptr<Layer>> layers;

		static Project* static_project_instance;
		static Renderer* static_renderer;
		static PhysicsSystem* static_physics_system;
		static AudioSystem* static_audio_system;
		static ScriptSystem* static_script_system;
	};
}