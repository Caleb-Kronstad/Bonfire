#pragma once

#include "Entity.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"
#include "ShadowMap.hpp"

namespace Bonfire
{
    class Scene
    {
    public:
        Scene(const std::string& path) : path(path) {}
        Scene(const Scene& other);

        bool LoadScene(ParamDatabase& param_database);
        bool SaveScene(ParamDatabase& param_database);
        void UpdateLightSources(Shader& shader, float shininess = 64.0f);

        std::unordered_map<uint32_t, std::shared_ptr<Entity>>& GetEntities() { return entities; }
        std::unordered_map<uint32_t, std::shared_ptr<Model>>& GetModels() { return models; }
        std::unordered_map<uint32_t, std::shared_ptr<Texture>>& GetTextures() { return textures; }
        std::unordered_map<uint32_t, std::shared_ptr<Material>>& GetMaterials() { return materials; }
        std::unordered_map<uint32_t, std::shared_ptr<Shader>>& GetShaders() { return shaders; }
        std::unordered_map<uint32_t, std::shared_ptr<PointLight>>& GetPointLights() { return point_lights; }
        std::unordered_map<uint32_t, std::shared_ptr<SpotLight>>& GetSpotLights() { return spot_lights; }
        std::unique_ptr<DirectionalLight>& GetDirectionalLight() { return directional_light; }
        std::unique_ptr<ShadowMap>& GetShadowMap() { return shadow_map; }
        std::unordered_map<uint32_t, std::shared_ptr<ModelComponent>>& GetModelComponents() { return model_components; }
        std::unordered_map<uint32_t, std::shared_ptr<LightSourceComponent>>& GetLightSourceComponents() { return light_source_components; }
        std::unordered_map<uint32_t, std::shared_ptr<PhysicsComponent>>& GetPhysicsComponents() { return physics_components; }
        std::unique_ptr<Camera>& GetEngineCamera() { return engine_camera; }
        std::unique_ptr<Skybox>& GetSkybox() { return skybox; }

    private:
        std::string path;
        
        std::unordered_map<uint32_t, std::shared_ptr<Entity>> entities;
        std::unordered_map<uint32_t, std::shared_ptr<Model>> models;
        std::unordered_map<uint32_t, std::shared_ptr<Texture>> textures;
        std::unordered_map<uint32_t, std::shared_ptr<Material>> materials;
        std::unordered_map<uint32_t, std::shared_ptr<Shader>> shaders;
        std::unordered_map<uint32_t, std::shared_ptr<PointLight>> point_lights;
        std::unordered_map<uint32_t, std::shared_ptr<SpotLight>> spot_lights;
        std::unordered_map<uint32_t, std::shared_ptr<ModelComponent>> model_components;
        std::unordered_map<uint32_t, std::shared_ptr<LightSourceComponent>> light_source_components;
        std::unordered_map<uint32_t, std::shared_ptr<PhysicsComponent>> physics_components;
        std::unique_ptr<DirectionalLight> directional_light;
        std::unique_ptr<Camera> engine_camera;
        std::unique_ptr<Skybox> skybox;
        std::unique_ptr<ShadowMap> shadow_map;
    };
}
