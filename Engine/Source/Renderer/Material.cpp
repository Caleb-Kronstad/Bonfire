#include "bonfire_pch.hpp"
#include "Material.hpp"

namespace Bonfire
{
    bool Material::AddTexture(std::shared_ptr<Texture> texture)
    {
        if (!texture) return false;

        size_t slot_index = static_cast<size_t>(texture->type);
        if (textures[slot_index] != nullptr)
        {
            Log::Warning("Material '" + name = "' already has a " + std::to_string(static_cast<int>(slot_index)) + " texture");
            return false;
        }

        textures[slot_index] = texture;
        return true;
    }

    bool Material::RemoveTexture(std::shared_ptr<Texture> texture)
    {
        if (!texture) return false;
        
        size_t slot_index = static_cast<size_t>(texture->type);
        if (textures[slot_index] == texture)
        {
            textures[slot_index] = nullptr;
            return true;
        }

        return false;
    }

    bool Material::HasTexture(TextureType type)
    {
        size_t slot_index = static_cast<size_t>(type);
        return textures[slot_index] != nullptr;
    }

    std::shared_ptr<Texture> Material::GetTexture(TextureType type)
    {
        size_t slot_index = static_cast<size_t>(type);
        return textures[slot_index];
    }
}