#pragma once

#include "Model.hpp"

namespace Bonfire
{
    struct Component
    {
        bool enabled;
        virtual ~Component() = default;
    };
    
    struct Transform : Component
    {
        static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::TRANSFORM;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        
        Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
        glm::quat GetOrientation();
        glm::mat4 GetTransformMatrix();
    };

    struct ModelData : Component
    {
        static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::MODEL_DATA;

        std::shared_ptr<Model> model;

        ModelData(std::shared_ptr<Model> model);
    };
}
