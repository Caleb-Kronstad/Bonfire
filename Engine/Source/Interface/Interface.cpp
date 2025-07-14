#include "bonfire_pch.hpp"
#include "Interface.hpp"

#include "Core/Utility.hpp"
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
		
		Project& project = Project::GetInstance();
		GLFWwindow* window = project.GetWindow().GetNativeWindow();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
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
	}
	void Interface::End()
	{
		ImGui::Render();
		ImGui::EndFrame();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Interface::OnUpdate()
	{
		Begin();

		Project& project = Project::GetInstance();
		Renderer& renderer = Project::GetRenderer();
		Window& window = project.GetWindow();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());

		ImGuiWindowFlags window_flags = 0;

		End();
	}

	void Interface::OnInput(Input& input)
	{
		if (m_BlockInputs)
		{
			ImGuiIO& io = ImGui::GetIO();
			input.Handled |= io.WantCaptureMouse;
			input.Handled |= io.WantCaptureKeyboard;
		}
	}

	void Interface::SetCustomStyle()
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

		style->Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		style->Colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);

		style->WindowRounding = 0.0f;
		style->ChildRounding = 0.0f;
		style->FrameRounding = 0.0f;
		style->GrabRounding = 0.0f;
		style->PopupRounding = 0.0f;
		style->ScrollbarRounding = 0.0f;
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