#pragma once

#include "Entity.hpp"
#include "Camera.hpp"

namespace Bonfire
{
    class Scene
    {
    public:
        Scene(const std::string& path) : path(path) { engine_camera = std::make_unique<Camera>(); }

        bool LoadScene(ParamDatabase& param_database);
        bool SaveScene(ParamDatabase& param_database);

        std::unordered_map<uint32_t, std::shared_ptr<Entity>>& GetEntities() { return entities; }
        std::unique_ptr<Camera>& GetEngineCamera() { return engine_camera; }

    private:
        std::string path;
        std::unordered_map<uint32_t, std::shared_ptr<Entity>> entities;
        std::unordered_map<uint32_t, std::shared_ptr<Model>> models;
        std::unordered_map<uint32_t, std::shared_ptr<Texture>> textures;
        std::unordered_map<uint32_t, std::shared_ptr<ModelComponent>> model_components;
        std::unordered_map<uint32_t, std::shared_ptr<TextureComponent>> texture_components;
        std::unique_ptr<Camera> engine_camera;
    };
}
