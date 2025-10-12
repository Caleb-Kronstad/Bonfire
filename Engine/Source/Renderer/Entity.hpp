#pragma once

#include "Shader.hpp"
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
}

namespace std
{
    template<>
    struct hash<Bonfire::EntityID>
    {
        size_t operator()(const Bonfire::EntityID& ref) const
        {
            return hash<uint32_t>()(ref.value);
        }
    };
}

namespace Bonfire {
    
    struct EntityData
    {
        bool enabled;
        std::string name;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        std::unordered_map<PARAM_TYPE, ParamReference> params;

        EntityData(bool enabled, std::string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
            : enabled(enabled), name(name), position(position), rotation(rotation), scale(scale)
        {
        }
        void AddParam(PARAM_TYPE type, ParamReference ref)
        {
            params.insert_or_assign(type, ref);
        }
    };

}
