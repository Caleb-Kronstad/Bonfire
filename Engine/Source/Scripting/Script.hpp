#pragma once

#include "Input/Input.hpp"
#include "Renderer/Entity.hpp"

namespace Bonfire
{
    class Script
    {
    public:
        // functions that are called at predetermined points in the project's run cycle
        virtual void Attach() {}
        virtual void Update(float delta_time) {}
        virtual void Interface() {}
        virtual void Input(Input& input) {}
        virtual void Detach() {}
        
    protected:
        // functions that can be called whenever the user pleases
        Entity* GetEntityOfName(const std::string& name);
        std::vector<Entity*> GetEntitiesOfName(const std::string& name);
        Entity* GetEntityOfId(const uint32_t& id);
    };
}
