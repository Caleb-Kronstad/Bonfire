#include "bonfire_pch.hpp"
#include "Engine.hpp"

namespace Bonfire
{
    Engine* Engine::static_engine_instance = nullptr;
    Renderer* Engine::static_renderer = nullptr;
    PhysicsManager* Engine::static_physics_system = nullptr;
    AudioSystem* Engine::static_audio_system = nullptr;
    ScriptSystem* Engine::static_script_system = nullptr;
    ThreadManager* Engine::static_thread_manager = nullptr;

    Engine::Engine(const std::string& project_name)
    {
        static_engine_instance = this;
        static_renderer = new Renderer();
        static_physics_system = new PhysicsManager();
        static_audio_system = new AudioSystem();
        static_script_system = new ScriptSystem();
        static_thread_manager = new ThreadManager();

        window = std::make_unique<Window>(WindowProperties(1280, 720, 0, 0, project_name));
    }

    void Engine::PushLayer(const std::shared_ptr<Layer>& layer)
    {
        layers.emplace_back(layer);
    }

    void Engine::PopLayer(const std::shared_ptr<Layer>& layer)
    {
        auto it = std::ranges::find(layers, layer);
        if (it != layers.end())
        {
            layer->OnDetach();
            layers.erase(it);
        }
    }

    void Engine::Run()
    {
        std::cout << "Current Project Path: " << std::filesystem::current_path() << "\n";

        LoadProjectConfig("Data/projectconfig.bonfire");

        window->window_props = WindowProperties(project_config.window_width, project_config.window_height, 0, 0,
                                                project_config.project_name);
        window->SetVSync(project_config.vsync);

        InitWindow();

        glfwSetKeyCallback(window->GetNativeWindow(), KeyCallbackDispatch);
        glfwSetMouseButtonCallback(window->GetNativeWindow(), MouseButtonCallbackDispatch);
        glfwSetCursorPosCallback(window->GetNativeWindow(), MouseCallbackDispatch);
        glfwSetScrollCallback(window->GetNativeWindow(), ScrollCallbackDispatch);
        glfwSetFramebufferSizeCallback(window->GetNativeWindow(), FramebufferSizeCallbackDispatch);

        InitOpenGL();

        static_audio_system->OnAttach();
        static_physics_system->OnAttach();

        static_script_system->OnAttach();
        static_script_system->ExecuteGlobalScript(project_config.project_manager_script_path);

        static_renderer->OnAttach();
        for (const std::string& scene_path : project_config.scene_paths)
        {
            std::unique_ptr<Scene> scene = std::make_unique<Scene>(scene_path);
            static_renderer->AddScene(std::move(scene));
        }
        static_renderer->GetParamDatabase().LoadParams();
        static_renderer->LoadScene(0);

        for (std::shared_ptr<Layer>& layer : layers)
            layer->OnAttach();

        glfwMakeContextCurrent(nullptr);

        static_thread_manager->Start();

        while (static_thread_manager->IsRunning())
        {
            glfwPollEvents();

            if (glfwWindowShouldClose(window->GetNativeWindow()))
            {
                static_thread_manager->Stop();
                break;
            }

            TickDeltaTime();

            if (project_running && static_renderer->GetScene().loaded)
            {
                static_script_system->OnUpdate(delta_time);
                static_audio_system->OnUpdate(delta_time);
            }

            for (const auto& layer : layers)
                layer->OnUpdate(delta_time);

            static_thread_manager->SignalRender();
            static_thread_manager->WaitForRenderComplete();
        }

        SetEditorRunState(false);
        SetProjectRunState(false);
        static_thread_manager->Shutdown();

        glfwMakeContextCurrent(window->GetNativeWindow());

        for (std::shared_ptr<Layer>& layer : layers)
            layer->OnDetach();

        static_renderer->OnDetach();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        static_script_system->OnDetach();
        static_audio_system->OnDetach();
        static_physics_system->OnDetach();

        glfwDestroyWindow(window->GetNativeWindow());
        glfwTerminate();
        delete static_script_system;
        delete static_audio_system;
        delete static_physics_system;
        delete static_renderer;
        delete static_thread_manager;
    }

    void Engine::InitOpenGL()
    {
        glfwMakeContextCurrent(window->GetNativeWindow());

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            Log::Error("Error Initializing GLAD");

        if (project_config.antialiasing_level > 0)
            glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        int actual_width, actual_height;
        glfwGetFramebufferSize(window->GetNativeWindow(), &actual_width, &actual_height);
        glViewport(0, 0, actual_width, actual_height);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    bool Engine::LoadProjectConfig(const std::string& config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
        {
            Log::Warning("[Project] No projectconfig.bonfire found, using defaults");
            return false;
        }

        try
        {
            nlohmann::json json;
            file >> json;

            project_config.project_name = json["project-name"].get<std::string>();
            project_config.window_width = json["window-width"].get<int>();
            project_config.window_height = json["window-height"].get<int>();
            project_config.fullscreen = json["fullscreen"].get<bool>();
            project_config.scene_paths = json["scene-paths"].get<std::vector<std::string>>();
            project_config.vsync = json["vsync"].get<bool>();
            project_config.project_manager_script_path = json["project-manager-script"].get<std::string>();
            project_config.antialiasing_level = json["antialiasing-level"].get<int>();
            project_config.shadow_resolution = json["shadow-resolution"].get<int>();

            if (project_config.scene_paths.empty())
            {
                Log::Warning("[Project] No scene path found, using defaults");
                std::filesystem::copy_file("Data/Editor/Defaults/defaultscene.bonfire",
                                           "Data/Scenes/defaultscene.bonfire");
                project_config.scene_paths.emplace_back("Data/Scenes/defaultscene.bonfire");
            }

            Log::Info("[Project] Loaded project config: " + project_config.project_name);
            return true;
        }
        catch (const std::exception& e)
        {
            Log::Error("[Project] Failed to parse projectconfig.bonfire " + std::string(e.what()));
            return false;
        }
    }

    void Engine::KeyCallback(GLFWwindow* glfw_window, int keycode, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            KeyPressedInput input(keycode);

            for (const auto& layer : layers)
                layer->OnInput(input);
            static_script_system->OnInput(input);
        }
        else if (action == GLFW_RELEASE)
        {
            KeyReleasedInput input(keycode);

            for (const auto& layer : layers)
                layer->OnInput(input);
            static_script_system->OnInput(input);
        }
    }

    void Engine::MouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            MouseButtonPressedInput input(button);

            for (const auto& layer : layers)
                layer->OnInput(input);
            static_script_system->OnInput(input);
        }
        else if (action == GLFW_RELEASE)
        {
            MouseButtonReleasedInput input(button);

            for (const auto& layer : layers)
                layer->OnInput(input);
            static_script_system->OnInput(input);
        }
    }

    void Engine::MouseCallback(GLFWwindow* glfw_window, double x_pos_in, double y_pos_in)
    {
        MouseMovedInput input(x_pos_in, y_pos_in);

        for (const auto& layer : layers)
            layer->OnInput(input);
        static_script_system->OnInput(input);
    }

    void Engine::ScrollCallback(GLFWwindow* glfw_window, double xoffset, double yoffset)
    {
        MouseScrolledInput input(xoffset, yoffset);

        for (const auto& layer : layers)
            layer->OnInput(input);
        static_script_system->OnInput(input);
    }

    void Engine::FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height)
    {
        window->GetWidth() = width;
        window->GetHeight() = height;
    }

    void Engine::InitWindow()
    {
        if (!glfwInit())
        {
            Log::Error("Error initializing GLFW");
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        if (project_config.antialiasing_level > 0)
            glfwWindowHint(GLFW_SAMPLES, project_config.antialiasing_level);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (project_config.fullscreen)
            window->SetNativeWindow(glfwCreateWindow(window->GetWidth(), window->GetHeight(),
                                                     project_config.project_name.c_str(), monitor, NULL));
        else
            window->SetNativeWindow(glfwCreateWindow(window->GetWidth(), window->GetHeight(),
                                                     project_config.project_name.c_str(), NULL, NULL));

        if (window->GetNativeWindow() == nullptr)
        {
            Log::Error("Error creating GLFW window");
            return;
        }

        if (!project_config.fullscreen)
        {
            glfwMaximizeWindow(window->GetNativeWindow());
            int actual_width, actual_height;
            glfwGetFramebufferSize(window->GetNativeWindow(), &actual_width, &actual_height);
            window->GetWidth() = actual_width;
            window->GetHeight() = actual_height;
        }
    }

    void Engine::TickDeltaTime()
    {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        delta_time = currentFrameTime - last_frame_time;
        last_frame_time = currentFrameTime;
    }
}
