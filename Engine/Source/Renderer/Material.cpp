#include "bonfire_pch.hpp"
#include "Material.hpp"

namespace Bonfire
{
    void Material::AddTexture(std::shared_ptr<Texture> texture)
    {
        textures.push_back(texture);
    }

    bool Material::RemoveTexture(std::shared_ptr<Texture> texture)
    {
        auto it = std::find(textures.begin(), textures.end(), texture);
        if (it != textures.end())
        {
            textures.erase(it);
            return true;
        }
        Log::Warning("Material does not contain this texture");
        return false;
    }
}