#pragma once

namespace Bonfire
{
    class Renderer;
    class PhysicsManager;
    class Window;
    
    class ThreadManager
    {
    public:
        ThreadManager();
        ~ThreadManager();
        
        bool IsRunning() const { return running.load(); } 
        
        void Start();
        void Stop() { running.store(false); }
        void Join();
        void Shutdown();
        void SignalRender();
        void WaitForRenderComplete();
        void WaitForRenderInit();
        
        std::unique_lock<std::mutex> LockPhysicsMutex() { return std::unique_lock<std::mutex>(physics_mutex); }
        
    private:
        void RenderThreadFunction();
        void PhysicsThreadFunction();
        
    private:
        std::thread render_thread;
        std::thread physics_thread;
        
        std::mutex render_mutex;
        std::mutex physics_mutex;
        std::mutex swap_mutex;
        std::mutex init_mutex;
        
        std::atomic<bool> running{true};
        std::atomic<bool> render_ready{false};
        std::atomic<bool> render_complete{true};
        std::atomic<bool> render_initialized{false};
        
        std::condition_variable render_condition;
        std::condition_variable swap_condition;
        std::condition_variable init_condition;
        
        static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
    };
}