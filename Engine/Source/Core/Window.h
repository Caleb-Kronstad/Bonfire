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
		Window(WindowProperties winProps = WindowProperties())
			: m_WindowProps(winProps)
		{
		}
		~Window() = default;

		void SetNativeWindow(GLFWwindow* window) { m_Window = window; }

		// Getters
		GLFWwindow* GetNativeWindow() { return m_Window; }
		unsigned int& GetWidth() { return m_WindowProps.Width; }
		unsigned int& GetHeight() { return m_WindowProps.Height; }
		unsigned int& GetXOffset() { return m_WindowProps.xOffset; }
		unsigned int& GetYOffset() { return m_WindowProps.yOffset; }
		std::string& GetTitle() { return m_WindowProps.Title; }
		
		// VSync
		void SetVSync(bool enabled) { m_IsVSync = enabled; }
		bool IsVSync() const { return m_IsVSync; }

	private:
		GLFWwindow* m_Window = nullptr;
		WindowProperties m_WindowProps;
		bool m_IsVSync = false;

	};
}