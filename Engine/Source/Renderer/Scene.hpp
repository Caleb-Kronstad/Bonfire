#pragma once

#include "Entity.hpp"

namespace Bonfire
{
    class Scene
    {
    public:
        Scene(const std::string& path) : path(path) {}

        bool Load();
        bool Save();

        std::vector<EntityID>& GetEntities() { return entities; }
        std::unordered_map<EntityID, EntityData>& GetEntitiesData() { return entities_data; }

    private:
        std::string path;
        std::vector<EntityID> entities;
        std::unordered_map<EntityID, EntityData> entities_data;
    };
}
