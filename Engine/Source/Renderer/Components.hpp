#pragma once

#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Material.hpp"

namespace Bonfire
{
    struct Component
    {
        uint32_t id = 0;
        bool enabled = true;

        virtual ~Component() = default;
    };
    
    struct ModelComponent : Component
    {
        std::shared_ptr<Model> model;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Material> material;

        ModelComponent() {}
        ModelComponent(uint32_t id, bool enabled, std::shared_ptr<Model> model, std::shared_ptr<Shader> shader, std::shared_ptr<Material> material)
        {
            this->id = id;
            this->enabled = enabled;
            this->model = model;
            this->shader = shader;
            this->material = material;
        }
    };

    struct PhysicsComponent : Component
    {

        PhysicsComponent() {}
    };

    struct AnimationComponent : Component
    {

        AnimationComponent() {}
    };
}
