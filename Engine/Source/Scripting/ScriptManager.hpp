#pragma once

#include "Script.hpp"

#include "Renderer/Scene.hpp"

namespace Bonfire
{
    
    class ScriptManager : Layer
    {
    public:
        ScriptManager() = default;
        ~ScriptManager() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(const float& delta_time) override;
        void OnInterfaceUpdate() override;
        void OnInput(Input& input) override;
        
        void ScriptsAttach();
        void ScriptsUpdate(float delta_time);
        void ScriptsInterface();
        void ScriptsInput(Input& input);
        void ScriptsDetach();
        
        std::vector<std::shared_ptr<Script>>& ScriptsGet() { return scripts; }
        
        void ScriptAdd(std::shared_ptr<Script> script) {
            if (script != nullptr)
            if (std::find(scripts.begin(), scripts.end(), script) == scripts.end())
                scripts.emplace_back(script);
        }
        void ScriptRemove(std::shared_ptr<Script> script) {
            if (script != nullptr)
            scripts.erase(std::remove_if(scripts.begin(), scripts.end(), [script](const std::shared_ptr<Script>& s) { return s == script; }), scripts.end());
        }

    private:
        std::vector<std::shared_ptr<Script>> scripts;
    };
}
