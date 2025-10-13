#pragma once

#include "Entity.hpp"
#include "Camera.hpp"

namespace Bonfire
{
    class Scene
    {
    public:
        Scene(const std::string& path) : path(path) {}

        bool LoadScene();
        bool SaveScene();

        std::vector<EntityID>& GetEntities() { return entities; }
        std::unordered_map<EntityID, EntityData>& GetEntitiesData() { return entities_data; }
        std::unique_ptr<Camera>& GetSceneCamera() { return camera; }

    private:
        std::string path;
        std::vector<EntityID> entities;
        std::unordered_map<EntityID, EntityData> entities_data;
        std::unique_ptr<Camera> camera;
    };
}
