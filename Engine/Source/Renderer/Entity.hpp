#pragma once

#include "Model.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

// entity components
#include "Components/Component.hpp"
#include "Components/Transform.hpp"

namespace Bonfire
{
    class Entity
    {
    public:
        Entity(std::string name = "NewEntity");

        bool AddComponent(std::shared_ptr<Component>& component);
        bool RemoveComponent(std::shared_ptr<Component>& component);

    public:
        std::string name;
        bool enabled;
        std::vector<std::shared_ptr<Component>> components;

    };
}
