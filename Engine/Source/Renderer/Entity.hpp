#pragma once

#include "Shader.hpp"
#include "Components.hpp"

namespace Bonfire
{
    struct EntityID {
        uint32_t value;

        explicit EntityID(uint32_t v) : value(v) {}

        bool operator==(const EntityID& other) const { return value == other.value; }
        bool operator!=(const EntityID& other) const { return value != other.value; }
        bool operator<(const EntityID& other) const { return value < other.value; }
    };

    struct ParamReference {
        uint32_t value;

        explicit ParamReference(uint32_t v) : value(v) {}

        bool operator==(const ParamReference& other) const { return value == other.value; }
        bool operator!=(const ParamReference& other) const { return value != other.value; }
    };

    struct Entity
    {
        Entity(const std::string& name = "NewEntity", glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
        

        bool enabled;
        std::string name;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::quat GetTransformOrientation();
        glm::mat4 GetTransformMatrix();

        std::unordered_map<PARAM_TYPE, ParamReference> params;
    };
}
