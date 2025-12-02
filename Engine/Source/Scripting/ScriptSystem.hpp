#pragma once

#include "LuaScript.hpp"
#include "Core/Layer.hpp"

#include "Renderer/Entity.hpp"
#include "Renderer/Scene.hpp"

namespace Bonfire
{
    
    class ScriptSystem : Layer
    {
    public:
        ScriptSystem();
        ~ScriptSystem();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(const float& delta_time) override;
        void OnInterfaceUpdate() override;
        void OnInput(Input& input) override;

        bool LoadScript(uint32_t id, const std::string& name, const std::string& path);
        bool UnloadScript(uint32_t id);
        std::shared_ptr<LuaScript> GetScript(uint32_t id);
        std::unordered_map<uint32_t, std::shared_ptr<LuaScript>>& GetLuaScripts() { return lua_scripts; }
        std::vector<std::shared_ptr<Layer>>& GetCppScripts() { return cpp_scripts; }

        void InitializeScripts(Scene& scene);
        void StartScripts(Scene& scene);
        void UpdateScripts(Scene& scene, float delta_time);
        void DestroyScripts(Scene& scene);

        void ExecuteGlobalScript(const std::string& script_path);

        void AttachCppScripts();
        void DetachCppScripts();

        lua_State* GetLuaState() { return lua_state; }

    private:
        void InitializeLua();
        void ShutdownLua();
        void RegisterBindings();

    private:
        lua_State* lua_state = nullptr;
        std::unordered_map<uint32_t, std::shared_ptr<LuaScript>> lua_scripts;
        std::vector<std::shared_ptr<Layer>> cpp_scripts;
        bool scripts_started = false;
    };
}
