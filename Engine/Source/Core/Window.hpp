#pragma once

namespace Bonfire
{
	struct WindowProperties
	{
		unsigned int Width;
		unsigned int Height;
		unsigned int xOffset;
		unsigned int yOffset;
		std::string Title;
		
		WindowProperties(unsigned int width = 1280, unsigned int height = 720, unsigned int xoffset = 0, unsigned int yoffset = 0, std::string title = "Window")
			: Width(width), Height(height), xOffset(xoffset), yOffset(yoffset), Title(title)
		{

		}
	};

	class Window
	{
	public:
		Window(WindowProperties winProps = WindowProperties());
		~Window();

		void SetNativeWindow(GLFWwindow* window) { glfw_window = window; }

		// Getters
		GLFWwindow* GetNativeWindow() const { return glfw_window; }
		unsigned int& GetWidth() { return window_props.Width; }
		unsigned int& GetHeight() { return window_props.Height; }
		unsigned int& GetXOffset() { return window_props.xOffset; }
		unsigned int& GetYOffset() { return window_props.yOffset; }
		std::string& GetTitle() { return window_props.Title; }
		
		// VSync
		void SetVSync(bool const enabled) { is_v_sync = enabled; }
		bool IsVSync() const { return is_v_sync; }

	public:
		WindowProperties window_props;

	private:
		GLFWwindow* glfw_window = nullptr;
		bool is_v_sync = false;

	};
}