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

		SetMainStyle();
		
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
		Project& project = Project::GetInstance();
		Renderer& renderer = project.GetRenderer();
		
		ImGui_ImplOpenGL3_NewFrame();
	    ImGui_ImplGlfw_NewFrame();
	    ImGui::NewFrame();
	    ImGuizmo::BeginFrame();

	    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar; // Add MenuBar flag
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

	    // Menu Bar
	    if (ImGui::BeginMenuBar())
	    {
	        if (ImGui::BeginMenu("File"))
	        {
	            if (ImGui::MenuItem("Reload", "Ctrl+L")) { renderer.Load(); }
	            if (ImGui::MenuItem("Save", "Ctrl+S")) { renderer.Save(); }
	            ImGui::Separator();
	            if (ImGui::MenuItem("Exit", "Alt+F4")) { project.SetEngineRunState(false); }
	            ImGui::EndMenu();
	        }
	        
	        if (ImGui::BeginMenu("Edit"))
	        {
	            if (ImGui::MenuItem("Undo", "Ctrl+Z")) { Log::Info("Undo"); }
	            if (ImGui::MenuItem("Redo", "Ctrl+Y")) { Log::Info("Redo"); }
	            ImGui::Separator();
	            if (ImGui::MenuItem("Cut", "Ctrl+X")) { Log::Info("Cut"); }
	            if (ImGui::MenuItem("Copy", "Ctrl+C")) { Log::Info("Copy"); }
	            if (ImGui::MenuItem("Paste", "Ctrl+V")) { Log::Info("Paste"); }
	            ImGui::EndMenu();
	        }
	        
	        if (ImGui::BeginMenu("Help"))
	        {
	            if (ImGui::MenuItem("Documentation")) { Log::Info("Documentation"); }
	            if (ImGui::MenuItem("About")) { Log::Info("Find more information at https://bonfireengine.com"); }
	            ImGui::EndMenu();
	        }
	        
	        ImGui::EndMenuBar();
	    }

	    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
	    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	    ImGui::End();
	}
	
	void Interface::End()
	{
		ImGui::Render();
		ImGui::EndFrame();
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

	void Interface::SetStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		
		colors[ImGuiCol_Text] = text_primary;
		colors[ImGuiCol_WindowBg] = background_primary;
		colors[ImGuiCol_Border] = background_tertiary;
		colors[ImGuiCol_FrameBg] = background_secondary;
		colors[ImGuiCol_FrameBgHovered] = highlight_primary;
		colors[ImGuiCol_FrameBgActive] = highlight_secondary;
		colors[ImGuiCol_TitleBg] = background_secondary;
		colors[ImGuiCol_TitleBgActive] = background_secondary;
		colors[ImGuiCol_TitleBgCollapsed] = background_secondary;
		colors[ImGuiCol_SliderGrab] = highlight_primary;
		colors[ImGuiCol_SliderGrabActive] = highlight_primary;
		colors[ImGuiCol_Header] = background_tertiary;
		colors[ImGuiCol_HeaderHovered] = highlight_primary;
		colors[ImGuiCol_HeaderActive] = highlight_primary;
		colors[ImGuiCol_TextSelectedBg] = highlight_primary;
		colors[ImGuiCol_CheckMark] = highlight_primary;
		colors[ImGuiCol_Button] = background_tertiary;
		colors[ImGuiCol_ButtonHovered] = highlight_secondary;
		colors[ImGuiCol_ButtonActive] = highlight_primary;
		colors[ImGuiCol_Separator] = background_tertiary;
		colors[ImGuiCol_SeparatorHovered] = highlight_primary;
		colors[ImGuiCol_SeparatorActive] = highlight_primary;
		colors[ImGuiCol_ResizeGrip] = highlight_primary;
		colors[ImGuiCol_ResizeGripHovered] = highlight_primary;
		colors[ImGuiCol_ResizeGripActive] = highlight_primary;
		colors[ImGuiCol_Tab] = background_primary;
		colors[ImGuiCol_TabHovered] = background_tertiary;
		colors[ImGuiCol_TabActive] = background_primary;
		colors[ImGuiCol_TabUnfocused] = background_primary;
		colors[ImGuiCol_TabUnfocusedActive] = background_primary;
		colors[ImGuiCol_DockingPreview] = highlight_primary;
		colors[ImGuiCol_DockingEmptyBg] = background_secondary;
		colors[ImGuiCol_PlotHistogram] = highlight_primary;
		colors[ImGuiCol_PlotHistogramHovered] = highlight_secondary;
		colors[ImGuiCol_DragDropTarget] = highlight_secondary;
		
		style->WindowRounding = 0.0f;
		style->FrameRounding = 0.0f;
		style->GrabRounding = 0.0f;
		style->PopupRounding = 0.0f;
		style->TabRounding = 0.0f;
		style->WindowMenuButtonPosition = ImGuiDir_Right;
		style->ScrollbarSize = 10.0f;
		style->GrabMinSize = 10.0f;
		style->DockingSeparatorSize = 1.0f;
		style->SeparatorTextBorderSize = 2.0f;
		style->WindowPadding = ImVec2(0.0f, 0.0f);
	}

	
	void Interface::SetMainStyle() 
	{
		text_primary = RgbaToImVec4(255, 255, 255);
		background_primary = RgbaToImVec4(38, 38, 38);
		background_secondary = RgbaToImVec4(23, 23, 23);
		background_tertiary = RgbaToImVec4(56, 56, 56);
		highlight_primary = RgbaToImVec4(116, 77, 169);
		highlight_secondary = RgbaToImVec4(141, 124, 192);

		SetStyle();
	}

	void Interface::SetGizmoStyle()
	{
		text_primary = RgbaToImVec4(0, 0, 0, 0);
		background_primary = RgbaToImVec4(0, 0, 0, 0);
		background_secondary = RgbaToImVec4(0, 0, 0, 0);
		background_tertiary = RgbaToImVec4(0, 0, 0, 0);
		highlight_primary = RgbaToImVec4(0, 0, 0, 0);
		highlight_secondary = RgbaToImVec4(0, 0, 0, 0);
		
		SetStyle();
	}
}