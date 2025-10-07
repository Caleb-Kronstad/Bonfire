#include "bonfire_pch.hpp"
#include "Interface.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
	Interface::Interface()
	{

	}
	Interface::~Interface()
	{

	}

	void Interface::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		SetCustomStyle();
		
		Project& project = Project::GetInstance();
		GLFWwindow* glfw_window = project.GetWindow().GetNativeWindow();

		ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}
	void Interface::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Interface::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		ImGui::End();
	}
	void Interface::End()
	{
		ImGui::Render();
		// ImGui::EndFrame();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Interface::OnUpdate()
	{
		// maybe this will be used for something someday
	}

	void Interface::OnInput(Input& input)
	{
		if (block_inputs)
		{
			ImGuiIO& io = ImGui::GetIO();
			input.Handled |= io.WantCaptureMouse;
			input.Handled |= io.WantCaptureKeyboard;
		}
	}

	void Interface::SetCustomStyle() // hard coded style for the time being, will update for customization later
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		
		ImVec4 text_primary = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		ImVec4 bg_darkest = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		ImVec4 bg_dark = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
		ImVec4 bg_main = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		ImVec4 bg_medium = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		ImVec4 bg_light = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		ImVec4 highlight = ImVec4(127.0f/255.0f, 84.0f/255.0f, 145.0f/255.0f, 1.00f);
		ImVec4 highlight_bright = ImVec4(0.83f, 0.57f, 0.94f, 1.00f);
		ImVec4 highlight_dark = ImVec4(0.549f, 0.259f, 0.675f, 1.0f);

		colors[ImGuiCol_Text] = text_primary;
		colors[ImGuiCol_WindowBg] = bg_main;
		colors[ImGuiCol_Border] = bg_darkest;
		colors[ImGuiCol_FrameBg] = bg_darkest;
		colors[ImGuiCol_FrameBgHovered] = highlight;
		colors[ImGuiCol_FrameBgActive] = bg_medium;
		colors[ImGuiCol_TitleBg] = bg_dark;
		colors[ImGuiCol_TitleBgActive] = bg_medium;
		colors[ImGuiCol_TitleBgCollapsed] = bg_darkest;
		colors[ImGuiCol_SliderGrab] = highlight;
		colors[ImGuiCol_SliderGrabActive] = highlight_bright;
		colors[ImGuiCol_Header] = bg_medium;
		colors[ImGuiCol_HeaderHovered] = highlight;
		colors[ImGuiCol_HeaderActive] = highlight;
		colors[ImGuiCol_TextSelectedBg] = highlight;
		colors[ImGuiCol_CheckMark] = highlight;
		colors[ImGuiCol_ButtonActive] = highlight;
		colors[ImGuiCol_Separator] = highlight;
		colors[ImGuiCol_SeparatorHovered] = highlight;
		colors[ImGuiCol_SeparatorActive] = highlight;
		colors[ImGuiCol_ResizeGrip] = highlight;
		colors[ImGuiCol_ResizeGripHovered] = highlight;
		colors[ImGuiCol_ResizeGripActive] = highlight;
		colors[ImGuiCol_Tab] = bg_dark;
		colors[ImGuiCol_TabHovered] = highlight;
		colors[ImGuiCol_TabActive] = bg_main;
		colors[ImGuiCol_TabUnfocused] = bg_darkest;
		colors[ImGuiCol_TabUnfocusedActive] = bg_dark;
		colors[ImGuiCol_DockingPreview] = highlight;
		colors[ImGuiCol_DockingEmptyBg] = bg_darkest;

		
		style->WindowRounding = 4.0f;
		style->FrameRounding = 4.0f;
		style->GrabRounding = 3.0f;
		style->PopupRounding = 4.0f;
		style->TabRounding = 4.0f;
		style->WindowMenuButtonPosition = ImGuiDir_Right;
		style->ScrollbarSize = 10.0f;
		style->GrabMinSize = 10.0f;
		style->DockingSeparatorSize = 1.0f;
		style->SeparatorTextBorderSize = 2.0f;
	}

	void Interface::SetClearStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(10, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 0);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;
		style->ButtonTextAlign = ImVec2(0, 0);

		style->Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}