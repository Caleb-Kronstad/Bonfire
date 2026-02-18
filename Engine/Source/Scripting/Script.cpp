#include "bonfire_pch.hpp"
#include "Script.hpp"

#include "Core/Engine.hpp"

namespace Bonfire
{
    uint32_t Script::EntityGetByName(std::string name)
    {
        Scene& scene = Engine::GetRenderer().GetScene();
        return scene.GetEntityOfName(name)->id;
    }

    std::vector<uint32_t> Script::EntityGetAllByName(std::string name)
    {
        Scene& scene = Engine::GetRenderer().GetScene();
        std::vector<uint32_t> entities;
        for (std::shared_ptr<Entity>& entity : scene.GetAllEntitiesOfName(name))
            entities.push_back(entity->id);
        return entities;
    }

    bool Script::EntitySetPosition(uint32_t entity, glm::vec3 position)
    {
        Scene& scene = Engine::GetRenderer().GetScene();
        scene.GetEntityById(entity)->position = position;
        return true;
    }

    bool Script::EntitySetRotation(uint32_t entity, glm::vec3 rotation)
    {
        Scene& scene = Engine::GetRenderer().GetScene();
        scene.GetEntityById(entity)->rotation = rotation;
        return true;
    }

    bool Script::EntitySetScale(uint32_t entity, glm::vec3 scale)
    {
        Scene& scene = Engine::GetRenderer().GetScene();
        scene.GetEntityById(entity)->scale = scale;
        return true;
    }
}
