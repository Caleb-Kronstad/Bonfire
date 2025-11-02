#pragma once

#include "Texture.hpp"

namespace Bonfire
{
    class Material
    {
    public:
        Material(const std::string& name) : name(name) {}

        bool AddTexture(std::shared_ptr<Texture> texture);
        bool RemoveTexture(std::shared_ptr<Texture> texture);
        bool HasTexture(TextureType type);
        std::shared_ptr<Texture> GetTexture(TextureType type);

        std::array<std::shared_ptr<Texture>, TEXTURE_TYPE_COUNT>& GetTextures() { return textures; }

    public:
        float shininess = 64.0f;
        uint32_t param_id = 0;
        std::string name;
        std::array<std::shared_ptr<Texture>, TEXTURE_TYPE_COUNT> textures;
        glm::vec2 texture_tiling = glm::vec2(1.0f, 1.0f);
        glm::vec2 texture_offset = glm::vec2(0.0f, 0.0f);
    };
}
