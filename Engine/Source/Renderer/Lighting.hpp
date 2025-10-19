#pragma once

namespace Bonfire
{
    struct LightSource
    {
        uint32_t id = 0;
        bool enabled = true;
        std::string name = "Light Source";

        virtual ~LightSource() = default;
    };

    struct DirectionalLight : LightSource
    {
        glm::vec3 direction;
        glm::vec3 color;

        DirectionalLight(glm::vec3 color = glm::vec3(255.0f), glm::vec3 direction = glm::vec3(0.0f))
            : color(color), direction(direction)
        {
            name = "Directional Light";
        }
    };

    struct PointLight : LightSource
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 scale;

        PointLight(glm::vec3 color = glm::vec3(255.0f), glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
            : color(color), position(position), scale(scale)
        {
            name = "Point Light";
        }
    };

    struct SpotLight : LightSource
    {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 scale;
        glm::vec3 direction;

        SpotLight(glm::vec3 color = glm::vec3(255.0f), glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 direction = glm::vec3(0.0f))
            : color(color), position(position), scale(scale), direction(direction)
        {
            name = "Spot Light";
        }
    };
}
