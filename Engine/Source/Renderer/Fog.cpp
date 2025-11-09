#include "bonfire_pch.hpp"
#include "Fog.hpp"

namespace Bonfire
{
    Fog::Fog()
    {
        
    }

    void Fog::ApplyToShader(Shader& shader)
    {
        shader.SetBool("fog_enabled", enabled);
        shader.SetVec3("fog_color", color);
        shader.SetFloat("fog_density", density);
        shader.SetFloat("fog_start", start);
        shader.SetFloat("fog_end", end);
        shader.SetInt("fog_type", static_cast<int>(type));
    }

}