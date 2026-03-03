#include "bonfire_pch.hpp"
#include "Script.hpp"

#include "Core/Engine.hpp"

namespace Bonfire
{
    Entity* Script::GetEntityOfName(const std::string& name)
    {
        return Engine::GetRenderer().GetScene().GetEntityOfName(name).get();
    }

    std::vector<Entity*> Script::GetEntitiesOfName(const std::string& name)
    {
        std::vector<Entity*> entities;
        for (std::shared_ptr<Entity> entity : Engine::GetRenderer().GetScene().GetEntitiesOfName(name))
        {
            entities.push_back(entity.get());
        }
        return entities;
    }

    Entity* Script::GetEntityOfId(const uint32_t& id)
    {
        return Engine::GetRenderer().GetScene().GetEntityOfId(id).get();
    }
}
