#pragma once

#include "Texture.hpp"
#include "Model.hpp"
#include "Shader.hpp"

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

        ModelComponent() {}
        ModelComponent(uint32_t id, bool enabled, std::shared_ptr<Model> model, std::shared_ptr<Shader> shader) { this->id = id; this->enabled = enabled; this->model = model; this->shader = shader; }
    };

    struct TextureComponent : Component
    {
        std::vector<std::shared_ptr<Texture>> textures;

        void AddTexture(std::shared_ptr<Texture> texture)
        {
            textures.push_back(texture);
            texture->Load();
        }
        bool RemoveTexture(std::shared_ptr<Texture> texture)
        {
            auto it = std::find(textures.begin(), textures.end(), texture);
            if (it != textures.end())
            {
                textures.erase(it);
                return true;
            }
            Log::Warning("[FAILED] Texture does not contain this Texture");
            return false;
        }

        TextureComponent() {}
        TextureComponent(uint32_t id, bool enabled, std::vector<std::shared_ptr<Texture>> textures) { this->id = id; this->enabled = enabled; this->textures = textures; }
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
