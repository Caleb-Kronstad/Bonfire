#pragma once

#include "Renderer/Shader.hpp"

namespace Bonfire
{
    class Fog
    {
    public:
        Fog();
        ~Fog() = default;

        void ApplyToShader(Shader& shader);

    public:
        bool enabled = false;
        glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);
        float density = 0.05f;
        float start = 10.0f;
        float end = 100.0f;
        FogType type = FogType::LINEAR;
    };
}
