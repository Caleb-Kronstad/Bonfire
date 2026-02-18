#pragma once

#include "ThreadManager.hpp"
#include "Input/Input.hpp"

#include "Core/Window.hpp"
#include "Core/Layer.hpp"

#include "Audio/AudioSystem.hpp"
#include "Scripting/ScriptManager.hpp"
#include "Renderer/Renderer.hpp"

namespace Bonfire
{
    inline constexpr uint32_t FIRST_ID = 1000000000;
    
	struct ProjectConfig
	{
		std::string project_name = "Bonfire Engine";
		std::string project_manager_script_path;
		int window_width = 1280;
		int window_height = 720;
		int shadow_resolution = 2048;
		int antialiasing_level = 4;
		bool fullscreen = true;
		bool vsync = true;
		std::vector<std::string> scene_paths;
	};
	
	class Engine
	{
	public:
		Engine(const std::string& project_name = "New Project");
		virtual ~Engine() = default;

		void Run();
		void PushLayer(const std::shared_ptr<Layer>& layer);
		void PopLayer(const std::shared_ptr<Layer>& layer);

		// Getters
		static Engine& Instance() { return *static_engine_instance; }
		static Renderer& GetRenderer() { return *static_renderer; }
		static PhysicsManager& GetPhysicsManager() { return *static_physics_system; }
		static AudioSystem& GetAudioManager() { return *static_audio_system; }
		static ScriptManager& GetScriptManager() { return *static_script_manager; }
		static ThreadManager& GetThreadManager() { return *static_thread_manager; }
		const std::string& GetProjectName() const { return project_config.project_name; }
		const float& GetDeltaTime() const { return delta_time; }
		Window& GetWindow() const { return *window; }
		std::vector<std::shared_ptr<Layer>>& GetLayers() { return layers; }
		
		const bool& GetProjectRunState() const { return project_running; }
		const bool& GetEditorRunState() const { return editor_running; }
		void SetProjectRunState(bool state) { project_running = state; }
		void SetEditorRunState(bool state) { editor_running = state; }

		// Callback functions
		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void MouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in);
		void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);
		void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

		static void KeyCallbackDispatch(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (static_engine_instance)
				static_engine_instance->KeyCallback(window, key, scancode, action, mods);
		}
		static void MouseButtonCallbackDispatch(GLFWwindow* window, int key, int action, int mods)
		{
			if (static_engine_instance)
				static_engine_instance->MouseButtonCallback(window, key, action, mods);
		}
		static void MouseCallbackDispatch(GLFWwindow* window, double xposin, double yposin)
		{
			if (static_engine_instance)
				static_engine_instance->MouseCallback(window, xposin, yposin);
		}
		static void ScrollCallbackDispatch(GLFWwindow* window, double xoffset, double yoffset)
		{
			if (static_engine_instance)
				static_engine_instance->ScrollCallback(window, xoffset, yoffset);
		}
		static void FramebufferSizeCallbackDispatch(GLFWwindow* window, int width, int height)
		{
			if (static_engine_instance)
				static_engine_instance->FramebufferSizeCallback(window, width, height);
		}

		const ProjectConfig& GetProjectConfig() const { return project_config; }

	private:
		void InitWindow();
		void InitOpenGL();
		void TickDeltaTime();
		bool LoadProjectConfig(const std::string& config_path);

	private:
		float delta_time = 0.0f;
		float last_frame_time = 0.0f;
		
		ProjectConfig project_config;
		std::unique_ptr<Window> window;
		
		bool editor_running = true;
		bool project_running = false;
		std::vector<std::shared_ptr<Layer>> layers;

		static Bonfire::Engine* static_engine_instance;
		static Bonfire::Renderer* static_renderer;
		static Bonfire::PhysicsManager* static_physics_system;
		static Bonfire::AudioSystem* static_audio_system;
		static Bonfire::ScriptManager* static_script_manager;
		static Bonfire::ThreadManager* static_thread_manager;
	};
}