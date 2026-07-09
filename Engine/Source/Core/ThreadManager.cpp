#include "bonfire_pch.hpp"
#include "ThreadManager.hpp"

#include "Core/Engine.hpp"

namespace Bonfire
{
    ThreadManager::ThreadManager()
    {
    }

    ThreadManager::~ThreadManager()
    {
        Shutdown();
    }

    void ThreadManager::Start()
    {
        physics_thread = std::thread(&ThreadManager::PhysicsThreadFunction, this);
        render_thread = std::thread(&ThreadManager::RenderThreadFunction, this);
    }

    void ThreadManager::Join()
    {
        if (physics_thread.joinable())
            physics_thread.join();
        if (render_thread.joinable())
            render_thread.join();
    }

    void ThreadManager::Shutdown()
    {
        Stop();
        {
            std::lock_guard<std::mutex> lock(render_mutex);
            render_ready.store(true);
        }
        render_condition.notify_all();
        Join();
    }

    void ThreadManager::PhysicsThreadFunction()
    {
        auto next_update = std::chrono::steady_clock::now();

        while (running.load())
        {
            if (Engine::Instance().GetProjectRunState() && Engine::GetRenderer().GetScene().loaded)
            {
                std::lock_guard<std::mutex> lock(physics_mutex);
                Engine::GetPhysicsManager().OnUpdate(FIXED_TIMESTEP);

                next_update += std::chrono::milliseconds(16);
                std::this_thread::sleep_until(next_update);
            }
            else
            {
                next_update = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }
    }

    void ThreadManager::RenderThreadFunction()
    {
        try
        {
            Engine& project = Engine::Instance();

            glfwMakeContextCurrent(project.GetWindow().GetNativeWindow());

            ImGuiIO& io = ImGui::GetIO();

            while (running.load())
            {
                std::unique_lock<std::mutex> lock(render_mutex);
                render_condition.wait(lock, [this]() { return render_ready.load() || !running.load(); });

                if (!running.load())
                    break;

                render_ready.store(false);
                lock.unlock();

                ImGui_ImplOpenGL3_NewFrame();
                //ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                ImGuizmo::BeginFrame();

                for (const auto& layer : project.GetLayers())
                    layer->OnInterfaceUpdate();
                Engine::GetScriptManager().OnInterfaceUpdate();

                Engine::GetRenderer().OnUpdate(project.GetDeltaTime());

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    GLFWwindow* backup_current_context = glfwGetCurrentContext();
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                    glfwMakeContextCurrent(backup_current_context);
                }

                glfwSwapBuffers(project.GetWindow().GetNativeWindow());

                {
                    std::lock_guard<std::mutex> swap_lock(swap_mutex);
                    render_complete.store(true);
                }
                swap_condition.notify_one();
            }

            glfwMakeContextCurrent(nullptr);
        }
        catch (const std::exception& e)
        {
            Log::Error("Render thread exception: " + std::string(e.what()));
        }
    }

    void ThreadManager::SignalRender()
    {
        {
            std::lock_guard<std::mutex> lock(render_mutex);
            render_ready.store(true);
            render_complete.store(false);
        }
        render_condition.notify_one();
    }

    void ThreadManager::WaitForRenderComplete()
    {
        std::unique_lock<std::mutex> lock(swap_mutex);
        swap_condition.wait(lock, [this]() { return render_complete.load(); });
    }

    void ThreadManager::WaitForRenderInit()
    {
        std::unique_lock<std::mutex> lock(init_mutex);
        init_condition.wait(lock, [this]() { return render_initialized.load(); });
    }
}
