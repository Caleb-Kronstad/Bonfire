#include "bonfire_pch.hpp"
#include "ScriptManager.hpp"

#include "Core/Engine.hpp"

namespace Bonfire
{
    void ScriptManager::OnAttach()
    {
    }
    void ScriptManager::OnDetach()
    {
    }

    void ScriptManager::OnUpdate(const float& delta_time)
    {
        if (Engine::Instance().GetProjectRunState())
            ScriptsUpdate(delta_time);
    }

    void ScriptManager::OnInterfaceUpdate()
    {
        if (Engine::Instance().GetProjectRunState())
            ScriptsInterface();
    }

    void ScriptManager::OnInput(Input& input)
    {
        if (Engine::Instance().GetProjectRunState())
            ScriptsInput(input);
    }

    void ScriptManager::ScriptsAttach()
    {
        for (std::shared_ptr<Script>& script : scripts)
            script->Attach();
    }

    void ScriptManager::ScriptsUpdate(float delta_time)
    {
        for (std::shared_ptr<Script>& script : scripts)
            script->Update(delta_time);
    }

    void ScriptManager::ScriptsInterface()
    {
        for (std::shared_ptr<Script>& script : scripts)
            script->Interface();
    }

    void ScriptManager::ScriptsInput(Input& input)
    {
        for (std::shared_ptr<Script>& script : scripts)
            script->Input(input);
    }
    
    void ScriptManager::ScriptsDetach()
    {
        for (std::shared_ptr<Script>& script : scripts)
            script->Detach();
    }
}
