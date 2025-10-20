#pragma once

#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Lighting.hpp"
#include "Physics/PhysicsObject.hpp"

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

    struct LightSourceComponent : Component
    {
        std::shared_ptr<LightSource> light_source;

        LightSourceComponent() {}
        LightSourceComponent(uint32_t id, bool enabled, std::shared_ptr<LightSource> light_source)
        {
            this->id = id;
            this->enabled = enabled;
            this->light_source = light_source;
        }
    };

    struct PhysicsComponent : Component
    {
        std::shared_ptr<PhysicsObject> physics_object;
        
        PhysicsComponent() {}
        PhysicsComponent(uint32_t id, bool enabled, std::shared_ptr<PhysicsObject> physics_object)
        {
            this->id = id;
            this->enabled = enabled;
            this->physics_object = physics_object;
        }
    };

    struct AnimationComponent : Component
    {

        AnimationComponent() {}
    };
}
