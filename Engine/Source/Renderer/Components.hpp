#pragma once

#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "Lighting.hpp"
#include "Physics/PhysicsBody.hpp"
#include "Animation/Animator.hpp"
#include "Audio/Audio.hpp"
#include "Scripting/LuaScript.hpp"

namespace Bonfire
{
    class LuaScript;
    
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
        std::shared_ptr<PhysicsBody> physics_body;
        
        PhysicsComponent() {}
        PhysicsComponent(uint32_t id, bool enabled, std::shared_ptr<PhysicsBody> physics_body)
        {
            this->id = id;
            this->enabled = enabled;
            this->physics_body = physics_body;
        }
    };

    struct AnimationComponent : Component
    {
        std::shared_ptr<Animator> animator;
        
        AnimationComponent() {}
        AnimationComponent(uint32_t id, bool enabled, std::shared_ptr<Animator> animator)
        {
            this->id = id;
            this->enabled = enabled;
            this->animator = animator;
        }
    };

    struct AudioComponent : Component
    {
        std::shared_ptr<Audio> audio;

        AudioComponent() {}
        AudioComponent(uint32_t id, bool enabled, std::shared_ptr<Audio> audio)
        {
            this->id = id;
            this->enabled = enabled;
            this->audio = audio;
        }
    };

    struct ScriptComponent : Component
    {
        std::shared_ptr<LuaScript> script;

        ScriptComponent() {}
        ScriptComponent(uint32_t id, bool enabled, std::shared_ptr<LuaScript> script)
        {
            this->id = id;
            this->enabled = enabled;
            this->script = script;
        }
    };
}
