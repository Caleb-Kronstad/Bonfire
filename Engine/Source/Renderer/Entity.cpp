#include "bonfire_pch.hpp"
#include "Entity.hpp"

namespace Bonfire
{
    Entity::Entity(std::string name)
        : name(name)
    {
        enabled = true;
    }

    bool Entity::AddComponent(std::shared_ptr<Component>& component)
    {
        auto it = std::find(components.begin(), components.end(), component);
        if (it == components.end())
        {
            components.push_back(component);
            return true;
        }
        Log::Warning("[FAILED] Component already added to Entity");
        return false;
    }
    bool Entity::RemoveComponent(std::shared_ptr<Component>& component)
    {
        auto it = std::find(components.begin(), components.end(), component);
        if (it != components.end())
        {
            components.erase(it);
            return true;
        }
        Log::Warning("[FAILED] Entity does not contain this Component");
        return false;
    }
}
