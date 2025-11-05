#include "bonfire_pch.hpp"
#include "ScriptSystem.hpp"

#include "LuaScript.hpp"
#include "LuaBindings.hpp"
#include "Renderer/Scene.hpp"

namespace Bonfire
{
    ScriptSystem::ScriptSystem() : Layer("ScriptSystem")
    {
    }

    ScriptSystem::~ScriptSystem()
    {
    }

    void ScriptSystem::OnAttach()
    {
        InitializeLua();
        Log::Info("ScriptSystem attached");
    }
    void ScriptSystem::OnDetach()
    {
        ShutdownLua();
        Log::Info("ScriptSystem detached");
    }

    void ScriptSystem::OnUpdate(const float& delta_time)
    {
        
    }

    void ScriptSystem::InitializeLua()
    {
        lua_state = luaL_newstate();
        if (!lua_state)
        {
            Log::Error("Failed to create Lua state");
            return;
        }

        luaL_openlibs(lua_state);
        RegisterBindings();

        Log::Info("Lua state initialized");
    }

    void ScriptSystem::ShutdownLua()
    {
        if (lua_state)
        {
            scripts.clear();
            lua_close(lua_state);
            lua_state = nullptr;
        }
    }

    void ScriptSystem::RegisterBindings()
    {
        LuaBindings::RegisterEntityBindings(lua_state);
        LuaBindings::RegisterVec3Bindings(lua_state);
        LuaBindings::RegisterInputBindings(lua_state);
    }

    bool ScriptSystem::LoadScript(uint32_t id, const std::string& name, const std::string& path)
    {
        std::shared_ptr<LuaScript> script = std::make_shared<LuaScript>(id, name, path);
        if (!script->Load(lua_state))
        {
            Log::Error("Failed to load script: " + name + " (" + path + ")");
            return false;
        }

        scripts.insert_or_assign(id, script);
        return true;
    }
    bool ScriptSystem::UnloadScript(uint32_t id)
    {
        if (scripts.contains(id))
        {
            scripts.erase(id);
            return true;
        }
        Log::Error("Failed to unload script: " + std::to_string(id) + " Script not found");
        return false;
    }
    std::shared_ptr<LuaScript> ScriptSystem::GetScript(uint32_t id)
    {
        if (scripts.contains(id))
            return scripts.at(id);
        Log::Error("Script not found: " + std::to_string(id));
        return nullptr;
    }

    void ScriptSystem::InitializeScripts(Scene& scene)
    {
        scripts_started = false;
    }
    
    void ScriptSystem::StartScripts(Scene& scene)
    {
        if (scripts_started) return;

        for (auto& [entity_id, entity] : scene.GetEntities())
        {
            if (!entity->HasComponent<ScriptComponent>()) continue;

            ScriptComponent& script_component = entity->GetComponent<ScriptComponent>();
            if (script_component.enabled && script_component.script)
            {
                script_component.script->CallOnStart(entity.get());
            }
        }

        scripts_started = true;
    }
    
    void ScriptSystem::UpdateScripts(Scene& scene, float delta_time)
    {
        if (!scripts_started) return;

        for (auto& [entity_id, entity] : scene.GetEntities())
        {
            if (!entity->enabled || !entity->HasComponent<ScriptComponent>()) continue;

            ScriptComponent& script_component = entity->GetComponent<ScriptComponent>();
            if (script_component.enabled && script_component.script)
            {
                script_component.script->CallOnUpdate(entity.get(), delta_time);
            }
        }
    }

    void ScriptSystem::DestroyScripts(Scene& scene)
    {
        if (!scripts_started) return;

        for (auto& [entity_id, entity] : scene.GetEntities())
        {
            if (!entity->HasComponent<ScriptComponent>()) continue;

            ScriptComponent& script_component = entity->GetComponent<ScriptComponent>();
            if (script_component.script)
            {
                script_component.script->CallOnDestroy(entity.get());
            }
        }
        scripts_started = false;
    }
}