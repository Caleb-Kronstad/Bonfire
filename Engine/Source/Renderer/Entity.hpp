#pragma once

#include "Shader.hpp"
#include "Components.hpp"
#include "ParamDatabase.hpp"

namespace Bonfire
{
    struct EntityID
    {
        uint32_t value;

        explicit EntityID(uint32_t v) : value(v) {}

        bool operator==(const EntityID& other) const { return value == other.value; }
        bool operator!=(const EntityID& other) const { return value != other.value; }
        bool operator<(const EntityID& other) const { return value < other.value; }
    };
    
    struct EntityData
    {
        bool enabled;
        std::string name;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        std::unordered_map<PARAM_TYPE, ParamReference> params;

        EntityData(bool enabled, std::string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        {
        }
        void AddParam(PARAM_TYPE type, ParamReference ref)
        {
            params[type] = ref;
        }
    };

}
