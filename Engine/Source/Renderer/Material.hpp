#pragma once

#include "Texture.hpp"

namespace Bonfire
{
    class Material
    {
    public:
        Material(const std::string& name) : name(name) {}

        void AddTexture(std::shared_ptr<Texture> texture);
        bool RemoveTexture(std::shared_ptr<Texture> texture);

        std::vector<std::shared_ptr<Texture>>& GetTextures() { return textures; }

    public:
        uint32_t param_id = 0;
        std::string name;
        std::vector<std::shared_ptr<Texture>> textures;
    };
}
