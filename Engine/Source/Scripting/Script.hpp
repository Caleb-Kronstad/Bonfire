#pragma once

#include "Input/Input.hpp"

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
        uint32_t EntityGetByName(std::string name);
        std::vector<uint32_t> EntityGetAllByName(std::string name);
        bool EntitySetPosition(uint32_t entity, glm::vec3 position);
        bool EntitySetRotation(uint32_t entity, glm::vec3 rotation);
        bool EntitySetScale(uint32_t entity, glm::vec3 scale);
    };
}
