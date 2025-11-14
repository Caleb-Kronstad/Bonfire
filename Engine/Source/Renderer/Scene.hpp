#pragma once

#include "Entity.hpp"
#include "Skybox.hpp"
#include "Camera.hpp"
#include "ShadowMap.hpp"
#include "Fog.hpp"

namespace Bonfire
{
    class Scene
    {
    public:
        Scene(const std::string& path) : path(path) {}
        Scene(const Scene& other);

        bool LoadScene(ParamDatabase& param_database);
        bool SaveScene(ParamDatabase& param_database);

        std::string SerializeToString(ParamDatabase& param_database);
        bool DeserializeFromString(const std::string& json_str, ParamDatabase& param_database);
        
        void UpdateLightSources(Shader& shader);
        bool SetCurrentCamera(uint32_t id);

        std::unordered_map<uint32_t, std::shared_ptr<Entity>>& GetEntities() { return entities; }
        std::unordered_map<uint32_t, std::shared_ptr<Model>>& GetModels() { return models; }
        std::unordered_map<uint32_t, std::shared_ptr<Texture>>& GetTextures() { return textures; }
        std::unordered_map<uint32_t, std::shared_ptr<Material>>& GetMaterials() { return materials; }
        std::unordered_map<uint32_t, std::shared_ptr<Shader>>& GetShaders() { return shaders; }
        std::unordered_map<uint32_t, std::shared_ptr<PointLight>>& GetPointLights() { return point_lights; }
        std::unordered_map<uint32_t, std::shared_ptr<SpotLight>>& GetSpotLights() { return spot_lights; }
        std::unordered_map<uint32_t, std::shared_ptr<ModelComponent>>& GetModelComponents() { return model_components; }
        std::unordered_map<uint32_t, std::shared_ptr<LightSourceComponent>>& GetLightSourceComponents() { return light_source_components; }
        std::unordered_map<uint32_t, std::shared_ptr<PhysicsComponent>>& GetPhysicsComponents() { return physics_components; }
        std::unordered_map<uint32_t, std::shared_ptr<AnimationComponent>>& GetAnimationComponents() { return animation_components; }
        std::unordered_map<uint32_t, std::shared_ptr<AudioComponent>>& GetAudioComponents() { return audio_components; }
        std::unordered_map<uint32_t, std::shared_ptr<ScriptComponent>>& GetScriptComponents() { return script_components; }
        std::unordered_map<uint32_t, std::shared_ptr<CameraComponent>>& GetCameraComponents() { return camera_components; }
        std::unique_ptr<DirectionalLight>& GetDirectionalLight() { return directional_light; }
        std::unique_ptr<ShadowMap>& GetShadowMap() { return shadow_map; }
        std::shared_ptr<Camera>& GetCurrentCamera() { return cameras.at(current_camera_id); }
        std::unordered_map<uint32_t, std::shared_ptr<Camera>>& GetCameras() { return cameras; }
        std::unique_ptr<Skybox>& GetSkybox() { return skybox; }
        std::unique_ptr<Fog>& GetFog() { return fog; }

        std::shared_ptr<Entity> GetEntityOfName(const std::string& name);
        std::vector<std::shared_ptr<Entity>> GetAllEntitiesOfName(const std::string& name);
        std::shared_ptr<Entity> GetEntityById(uint32_t id);

    public:
        std::string path;
        
    private:
        
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
        std::unordered_map<uint32_t, std::shared_ptr<AnimationComponent>> animation_components;
        std::unordered_map<uint32_t, std::shared_ptr<AudioComponent>> audio_components;
        std::unordered_map<uint32_t, std::shared_ptr<ScriptComponent>> script_components;
        std::unordered_map<uint32_t, std::shared_ptr<CameraComponent>> camera_components;
        std::unique_ptr<DirectionalLight> directional_light;
        std::unordered_map<uint32_t, std::shared_ptr<Camera>> cameras;
        uint32_t current_camera_id = 0;
        std::unique_ptr<Skybox> skybox;
        std::unique_ptr<ShadowMap> shadow_map;
        std::unique_ptr<Fog> fog;
    };
}
