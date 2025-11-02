#include "bonfire_pch.hpp"
#include "Scene.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"
#include "Physics/PhysicsSystem.hpp"

namespace Bonfire
{
    void Scene::UpdateLightSources(Shader& shader)
    {
        if (directional_light != nullptr)
        {
            shader.SetVec3("directional_light.direction", directional_light->direction);
            shader.SetVec3("directional_light.ambient", directional_light->color / 255.0f * 0.2f);
            shader.SetVec3("directional_light.diffuse", directional_light->color / 255.0f);
            shader.SetVec3("directional_light.specular", directional_light->color / 255.0f * 0.2f);
        }

        int i = 0;
        for (auto& [id, point_light] : point_lights)
        {
            std::string number = std::to_string(i);

            shader.SetVec3("point_lights[" + number + "].position", point_light->position);
            shader.SetVec3("point_lights[" + number + "].ambient", point_light->color / 255.0f * 0.1f * point_light->intensity);
            shader.SetVec3("point_lights[" + number + "].diffuse", point_light->color / 255.0f * point_light->intensity);
            shader.SetVec3("point_lights[" + number + "].specular", point_light->color / 255.0f * point_light->intensity);

            shader.SetFloat("point_lights[" + number + "].constant", 1.0f);
            shader.SetFloat("point_lights[" + number + "].linear", 0.09f);
            shader.SetFloat("point_lights[" + number + "].quadratic", 0.032f);
            i++;
        }
        shader.SetInt("num_point_lights", i);

        i = 0;
        for (auto& [id, spot_light] : spot_lights)
        {
            std::string number = std::to_string(i);

            shader.SetVec3("spot_lights[" + number + "].position", spot_light->position);
            shader.SetVec3("spot_lights[" + number + "].direction", spot_light->direction);
            shader.SetVec3("spot_lights[" + number + "].ambient", spot_light->color / 255.0f * 0.1f);
            shader.SetVec3("spot_lights[" + number + "].diffuse", spot_light->color / 255.0f);
            shader.SetVec3("spot_lights[" + number + "].specular", spot_light->color / 255.0f * 0.5f);

            shader.SetFloat("spotLights[" + number + "].cutOff", glm::cos(glm::radians(12.5f)));
            shader.SetFloat("spotLights[" + number + "].outerCutOff", glm::cos(glm::radians(25.0f)));
            shader.SetFloat("spotLights[" + number + "].constant", 1.0f);
            shader.SetFloat("spotLights[" + number + "].linear", 0.09f);
            shader.SetFloat("spotLights[" + number + "].quadratic", 0.032f);
            i++;
        }
        shader.SetInt("num_spot_lights", i);
    }
    
    bool Scene::LoadScene(ParamDatabase& param_database)
    {
        entities.clear();
        models.clear();
        textures.clear();
        shaders.clear();
        materials.clear();
        point_lights.clear();
        spot_lights.clear();
        model_components.clear();
        light_source_components.clear();
        physics_components.clear();
        animation_components.clear();
        audio_components.clear();
        directional_light = nullptr;
        current_camera = nullptr;
        skybox = nullptr;
        shadow_map = nullptr;

        // LOAD COMPONENT TYPES FROM PARAM DATABASE
        for (auto& [model_id, model_data] : param_database.model_params)
        {
            std::shared_ptr<Model> model;
            if (model_data.is_animated)
            {
                model = std::make_shared<SkeletalModel>(model_data.path);
                SkeletalModel* skel_model = static_cast<SkeletalModel*>(model.get());
                if (skel_model->GetAnimations().empty())
                    Log::Warning("SkeletalModel created but no animations loaded!");
                else
                    Log::Info("Successfully loaded " + std::to_string(skel_model->GetAnimations().size()) + " animations");
            }
            else
            {
                model = std::make_shared<Model>(model_data.path);
                model->Load();
            }
            model->param_id = model_id;
            model->name = model_data.name;
            models.insert_or_assign(model_id, std::move(model));
        }
        for (auto& [texture_id, texture_data] : param_database.texture_params)
        {
            std::shared_ptr<Texture> texture = std::make_shared<Texture>(texture_data.path, texture_data.type, texture_data.flip);
            texture->name = texture_data.name;
            texture->param_id = texture_id;
            texture->Load();
            textures.insert_or_assign(texture_id, std::move(texture));
        }
        for (auto& [material_id, material_data] : param_database.material_params)
        {
            std::shared_ptr<Material> material = std::make_shared<Material>(material_data.name);
            material->param_id = material_id;

            if (textures.find(material_data.diffuse_id) != textures.end())
                material->textures[static_cast<size_t>(TextureType::DIFFUSE)] = textures.at(material_data.diffuse_id);
            if (textures.find(material_data.specular_id) != textures.end())
                material->textures[static_cast<size_t>(TextureType::SPECULAR)] = textures.at(material_data.specular_id);
            if (textures.find(material_data.normal_id) != textures.end())
                material->textures[static_cast<size_t>(TextureType::NORMAL)] = textures.at(material_data.normal_id);
            if (textures.find(material_data.height_id) != textures.end())
                material->textures[static_cast<size_t>(TextureType::HEIGHT)] = textures.at(material_data.height_id);
            if (textures.find(material_data.emission_id) != textures.end())
                material->textures[static_cast<size_t>(TextureType::EMISSION)] = textures.at(material_data.emission_id);

            material->shininess = material_data.shininess;
            material->texture_tiling = material_data.tiling;
            material->texture_offset = material_data.offset;
            materials.insert_or_assign(material_id, std::move(material));
        }
        for (auto& [shader_id, shader_data] : param_database.shader_params)
        {
            std::shared_ptr<Shader> shader = std::make_shared<Shader>(shader_id, shader_data.name, shader_data.vert_path, shader_data.frag_path, shader_data.geom_path);
            shader->Load();
            shaders.insert_or_assign(shader_id, std::move(shader));
        }
        for (auto& [audio_id, audio_data] : param_database.audio_params)
        {
            std::shared_ptr<Audio> audio = std::make_shared<Audio>(audio_id, audio_data.path);
            Project::GetAudioSystem().AddAudio(audio);
        }
        //  LOAD OTHER PARAM TYPES

        std::ifstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open scene file: " + path);
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json_string = buffer.str();

        current_camera = std::make_unique<Camera>(0);
        directional_light = std::make_unique<DirectionalLight>();

        if (!DeserializeFromString(json_string, param_database))
        {
            Log::Error("Error deserializing scene file at " + path);
            return false;
        }

        Log::Info("Loaded scene from " + path);
        return true;
    }

    bool Scene::SaveScene(ParamDatabase& param_database)
    {
        std::string json_string = SerializeToString(param_database);

        std::ofstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open scene file for writing: " + path);
            return false;
        }

        try
        {
            file << json_string;
        }
        catch (const std::exception& e)
        {
            Log::Error("Failed to write scene to file: " + std::string(e.what()));
            return false;
        }

        Log::Info("Saved scene to " + path);
        return true;
    }

    std::string Scene::SerializeToString(ParamDatabase& param_database)
    {
        nlohmann::json json;
        nlohmann::json camera_array = nlohmann::json::array();
        nlohmann::json entities_array = nlohmann::json::array();

        nlohmann::json camera_json;
        camera_json["id"] = current_camera->GetID();
        camera_json["yaw"] = current_camera->yaw;
        camera_json["pitch"] = current_camera->pitch;
        camera_json["position"] = {current_camera->position.x, current_camera->position.y,
            current_camera->position.z};
        camera_json["up"] = {current_camera->GetWorldUpVector().x, current_camera->GetWorldUpVector().y,
            current_camera->GetWorldUpVector().z};
        camera_array.push_back(camera_json);

        nlohmann::json directional_light_json;
        directional_light_json["id"] = directional_light->id;
        directional_light_json["name"] = directional_light->name;
        directional_light_json["color"] = {directional_light->color.x, directional_light->color.y,
            directional_light->color.z};
        directional_light_json["direction"] = {directional_light->direction.x, directional_light->direction.y,
            directional_light->direction.z};

        nlohmann::json components_json;

        nlohmann::json models_json;
        for (const auto& [id, model_component] : model_components)
        {
            nlohmann::json model_json;
            model_json["enabled"] = model_component->enabled;
            model_json["model-id"] = model_component->model->param_id;
            model_json["shader-id"] = model_component->shader->param_id;
            model_json["material-id"] = model_component->material->param_id;
            models_json[std::to_string(id)] = model_json;
        }
        if (!models_json.empty())
            components_json["models"] = models_json;

        nlohmann::json lights_json;
        for (const auto& [id, light_component] : light_source_components)
        {
            nlohmann::json light_json;
            light_json["enabled"] = light_component->enabled;
            light_json["light-id"] = light_component->light_source->id;
            light_json["light-enabled"] = light_component->light_source->enabled;

            if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_component->light_source))
            {
                light_json["light-type"] = "point";
                light_json["color"] = {point_light->color.x, point_light->color.y, point_light->color.z};
                light_json["position"] = {point_light->position.x, point_light->position.y, point_light->position.z};
                light_json["scale"] = {point_light->scale.x, point_light->scale.y, point_light->scale.z};
                light_json["intensity"] = point_light->intensity;
            }
            else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_component->light_source))
            {
                light_json["light-type"] = "spot";
                light_json["color"] = {spot_light->color.x, spot_light->color.y, spot_light->color.z};
                light_json["position"] = {spot_light->position.x, spot_light->position.y, spot_light->position.z};
                light_json["scale"] = {spot_light->scale.x, spot_light->scale.y, spot_light->scale.z};
                light_json["direction"] = {spot_light->direction.x, spot_light->direction.y, spot_light->direction.z};
            }

            lights_json[std::to_string(id)] = light_json;
        }
        if (!lights_json.empty())
            components_json["light_sources"] = lights_json;

        nlohmann::json physics_json;
        for (const auto& [id, physics_component] : physics_components)
        {
            nlohmann::json phys_json;
            phys_json["enabled"] = physics_component->enabled;
            phys_json["physics-id"] = physics_component->physics_body->id;
            phys_json["physics-enabled"] = physics_component->physics_body->enabled;
            phys_json["physics-name"] = physics_component->physics_body->name;
            phys_json["body-type"] = physics_component->physics_body->GetBodyType();

            auto shape_data = physics_component->physics_body->GetShapeData();
            phys_json["shape-type"] = shape_data.type;
            phys_json["dimensions"] = {shape_data.dimensions.x, shape_data.dimensions.y, shape_data.dimensions.z};

            physics_json[std::to_string(id)] = phys_json;
        }
        if (!physics_json.empty())
            components_json["physics"] = physics_json;

        nlohmann::json animations_json;
        for (const auto& [id, animation_component] : animation_components)
        {
            nlohmann::json animation_json;
            animation_json["enabled"] = animation_component->enabled;
            animation_json["current-animation"] = animation_component->animator->GetCurrentAnimationName();
            animations_json[std::to_string(id)] = animation_json;
        }
        if (!animations_json.empty())
            components_json["animations"] = animations_json;

        nlohmann::json audios_json;
        for (const auto& [id, audio_component] : audio_components)
        {
            nlohmann::json audio_json;
            audio_json["enabled"] = audio_component->enabled;
            audio_json["loop"] = audio_component->audio->GetLoop();
            audio_json["volume"] = audio_component->audio->GetVolume();
            audio_json["pitch"] = audio_component->audio->GetPitch();
            audio_json["play-on-awake"] = audio_component->audio->GetPlayOnAwake();
            audio_json["audio-id"] = audio_component->audio->id;
            audios_json[std::to_string(id)] = audio_json;
        }
        if (!audios_json.empty())
            components_json["audios"] = audios_json;

        for (const auto& [id, entity] : entities)
        {
            nlohmann::json entity_json;
            entity_json["id"] = id;
            entity_json["enabled"] = entity->enabled;
            entity_json["name"] = entity->name;

            entity_json["position"] = {entity->position.x, entity->position.y, entity->position.z};
            entity_json["rotation"] = {entity->rotation.x, entity->rotation.y, entity->rotation.z};
            entity_json["scale"] = {entity->scale.x, entity->scale.y, entity->scale.z};
            entity_json["parent"] = entity->parent;

            nlohmann::json entity_components_json;
            if (entity->HasComponent<ModelComponent>())
            {
                ModelComponent& model_component = entity->GetComponent<ModelComponent>();
                entity_components_json["model_component"] = model_component.id;
            }
            if (entity->HasComponent<LightSourceComponent>())
            {
                LightSourceComponent& light_source_component = entity->GetComponent<LightSourceComponent>();
                entity_components_json["light_source_component"] = light_source_component.id;
            }
            if (entity->HasComponent<PhysicsComponent>())
            {
                PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
                entity_components_json["physics_component"] = physics_component.id;
            }
            if (entity->HasComponent<AnimationComponent>())
            {
                AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
                entity_components_json["animation_component"] = animation_component.id;
            }
            if (entity->HasComponent<AudioComponent>())
            {
                AudioComponent& audio_component = entity->GetComponent<AudioComponent>();
                entity_components_json["audio_component"] = audio_component.id;
            }

            entity_json["components"] = entity_components_json;
            entities_array.push_back(entity_json);
        }

        json["DATA-TYPE"]["type"] = "SCENE";
        json["cameras"] = camera_array;
        json["directional_light"] = directional_light_json;
        json["components"] = components_json;
        json["entities"] = entities_array;

        return json.dump();
    }

    bool Scene::DeserializeFromString(const std::string& json_str, ParamDatabase& param_database)
    {
        entities.clear();
        point_lights.clear();
        spot_lights.clear();
        model_components.clear();
        light_source_components.clear();
        animation_components.clear();
        physics_components.clear();
        audio_components.clear();

        nlohmann::json json;
        try
        {
            json = nlohmann::json::parse(json_str);
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse scene JSON from string: " + std::string(e.what()));
            return false;
        }

        if (!json.contains("DATA-TYPE"))
        {
            Log::Error("Unknown data type when trying to load scene from snapshot");
            return false;
        }
        std::string data_type = json["DATA-TYPE"]["type"].get<std::string>();
        if (data_type != "SCENE")
        {
            Log::Error("Invalid data type: expected 'SCENE', got '" + data_type + "'");
            return false;
        }

        if (json.contains("cameras"))
        {
            if (!current_camera)
            {
                Log::Error("Cannot deserialize: camera not initialized. Load scene first.");
                return false;
            }
            for (const auto& camera_json : json["cameras"])
            {
                current_camera->SetID(camera_json["id"].get<uint32_t>());
                current_camera->yaw = camera_json["yaw"].get<float>();
                current_camera->pitch = camera_json["pitch"].get<float>();

                auto position_array = camera_json["position"].get<std::vector<float>>();
                current_camera->position = glm::vec3(position_array[0], position_array[1], position_array[2]);

                auto up_array = camera_json["up"].get<std::vector<float>>();
                current_camera->SetWorldUpVector(glm::vec3(up_array[0], up_array[1], up_array[2]));

                current_camera->UpdateCameraVectors();
            }
        }

        if (json.contains("directional_light"))
        {
            const auto& dir_light_json = json["directional_light"];
            directional_light->id = dir_light_json["id"].get<uint32_t>();
            directional_light->name = dir_light_json["name"].get<std::string>();

            auto color_array = dir_light_json["color"].get<std::vector<float>>();
            directional_light->color = glm::vec3(color_array[0], color_array[1], color_array[2]);

            auto direction_array = dir_light_json["direction"].get<std::vector<float>>();
            directional_light->direction = glm::vec3(direction_array[0], direction_array[1], direction_array[2]);
        }

        if (json.contains("components"))
        {
            const auto& components = json["components"];

            if (components.contains("models"))
            {
                for (const auto& [model_component_id, model_data] : components["models"].items())
                {
                    uint32_t id = std::stoul(model_component_id);
                    bool enabled = model_data["enabled"].get<bool>();
                    uint32_t model_id = model_data["model-id"].get<uint32_t>();
                    uint32_t shader_id = model_data["shader-id"].get<uint32_t>();
                    uint32_t material_id = model_data["material-id"].get<uint32_t>();
                    
                    std::shared_ptr<Model> model = models.at(model_id);
                    std::shared_ptr<Shader> shader = shaders.at(shader_id);
                    std::shared_ptr<Material> material = materials.at(material_id);
                    
                    if (model && shader && material)
                    {
                        std::shared_ptr<ModelComponent> model_component = std::make_shared<ModelComponent>(id, enabled, model, shader, material);
                        model_components.insert_or_assign(id, model_component);
                    }
                }
            }

            if (components.contains("light_sources"))
            {
                for (const auto& [light_component_id, light_data] : components["light_sources"].items())
                {
                    uint32_t id = std::stoul(light_component_id);
                    bool enabled = light_data["enabled"].get<bool>();
                    std::string light_type = light_data["light-type"].get<std::string>();

                    std::shared_ptr<LightSource> light_source = nullptr;

                    if (light_type == "point")
                    {
                        auto color_array = light_data["color"].get<std::vector<float>>();
                        auto position_array = light_data["position"].get<std::vector<float>>();
                        auto scale_array = light_data["scale"].get<std::vector<float>>();
                        glm::vec3 color(color_array[0], color_array[1], color_array[2]);
                        glm::vec3 position(position_array[0], position_array[1], position_array[2]);
                        glm::vec3 scale(scale_array[0], scale_array[1], scale_array[2]);

                        std::shared_ptr<PointLight> point_light = std::make_shared<PointLight>(color, position,
                            scale);
                        point_light->id = light_data["light-id"].get<uint32_t>();
                        point_light->enabled = light_data["light-enabled"].get<bool>();
                        if (light_data.contains("intensity"))
                            point_light->intensity = light_data["intensity"].get<float>();
                        light_source = point_light;
                    }
                    else if (light_type == "spot")
                    {
                        auto color_array = light_data["color"].get<std::vector<float>>();
                        auto position_array = light_data["position"].get<std::vector<float>>();
                        auto scale_array = light_data["scale"].get<std::vector<float>>();
                        auto direction_array = light_data["direction"].get<std::vector<float>>();
                        glm::vec3 color(color_array[0], color_array[1], color_array[2]);
                        glm::vec3 position(position_array[0], position_array[1], position_array[2]);
                        glm::vec3 scale(scale_array[0], scale_array[1], scale_array[2]);
                        glm::vec3 direction(direction_array[0], direction_array[1], direction_array[2]);

                        std::shared_ptr<SpotLight> spot_light = std::make_shared<SpotLight>(color, position, scale, direction);
                        spot_light->id = light_data["light-id"].get<uint32_t>();
                        spot_light->enabled = light_data["light-enabled"].get<bool>();
                        light_source = spot_light;
                    }

                    if (light_source)
                    {
                        std::shared_ptr<LightSourceComponent> light_component =
                            std::make_shared<LightSourceComponent>(id, enabled, light_source);
                        light_source_components.insert_or_assign(id, light_component);
                    }
                }
            }

            if (components.contains("physics"))
            {
                for (const auto& [physics_component_id, physics_data] : components["physics"].items())
                {
                    PhysicsSystem& physics_system = Project::GetPhysicsSystem();

                    uint32_t id = std::stoul(physics_component_id);
                    bool enabled = physics_data["enabled"].get<bool>();

                    uint32_t physics_id = physics_data["physics-id"].get<uint32_t>();
                    bool physics_enabled = physics_data["physics-enabled"].get<bool>();
                    std::string physics_name = physics_data["physics-name"].get<std::string>();
                    PhysicsBodyType body_type = static_cast<PhysicsBodyType>(physics_data["body-type"].get<uint8_t>());
                    PhysicsShapeType shape_type = static_cast<PhysicsShapeType>(physics_data["shape-type"].get<uint8_t>());

                    auto dims_array = physics_data["dimensions"].get<std::vector<float>>();
                    glm::vec3 dimensions(dims_array[0], dims_array[1], dims_array[2]);

                    std::shared_ptr<PhysicsBody> physics_body;

                    if (shape_type == PhysicsShapeType::BOX)
                        physics_body = physics_system.CreateBoxBody(glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), dimensions, body_type);
                    else if (shape_type == PhysicsShapeType::SPHERE)
                        physics_body = physics_system.CreateSphereBody(glm::vec3(0.0f), dimensions.x, body_type);
                    else if (shape_type == PhysicsShapeType::CAPSULE)
                        physics_body = physics_system.CreateCapsuleBody(glm::vec3(0.0f), glm::quat(glm::vec3(0.0f)), dimensions.x, dimensions.y, body_type);

                    if (physics_body)
                    {
                        physics_body->id = physics_id;
                        physics_body->enabled = physics_enabled;
                        physics_body->SetEnabled(enabled);
                        physics_body->name = physics_name;

                        std::shared_ptr<PhysicsComponent> physics_component = std::make_shared<PhysicsComponent>(id, enabled, physics_body);
                        physics_components.insert_or_assign(id, physics_component);
                    }
                }
            }
            
            if (components.contains("animations"))
            {
                for (const auto& [animation_component_id, animation_data] : components["animations"].items())
                {
                    uint32_t id = std::stoul(animation_component_id);
                    bool enabled = animation_data["enabled"].get<bool>();
                    std::string current_animation = animation_data["current-animation"].get<std::string>();

                    std::shared_ptr<Animator> animator;
                    std::shared_ptr<AnimationComponent> animation_component = std::make_shared<AnimationComponent>(id, enabled, animator);

                    animation_components.insert_or_assign(id, animation_component);
                }
            }

            if (components.contains("audios"))
            {
                for (const auto& [audio_component_id, audio_data] : components["audios"].items())
                {
                    uint32_t id = std::stoul(audio_component_id);
                    bool enabled = audio_data["enabled"].get<bool>();
                    bool loop = audio_data["loop"].get<bool>();
                    float volume = audio_data["volume"].get<float>();
                    float pitch = audio_data["pitch"].get<float>();
                    bool play_on_awake = audio_data["play-on-awake"].get<bool>();
                    uint32_t audio_id = audio_data["audio-id"].get<uint32_t>();

                    std::shared_ptr<Audio> audio = Project::GetAudioSystem().GetAudio(audio_id);

                    if (audio)
                    {
                        audio->SetLoop(loop);
                        audio->SetVolume(volume);
                        audio->SetPitch(pitch);
                        audio->SetPlayOnAwake(play_on_awake);
                        std::shared_ptr<AudioComponent> audio_component = std::make_shared<AudioComponent>(id, enabled, audio);
                        audio_components.insert_or_assign(id, audio_component);
                    }
                }
            }

            for (auto& [id, light_component] : light_source_components)
            {
                if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_component->light_source))
                    point_lights.insert_or_assign(point_light->id, point_light);
                else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_component->light_source))
                    spot_lights.insert_or_assign(spot_light->id, spot_light);
            }
        }

        if (json.contains("entities"))
        {
            for (const auto& entity_json : json["entities"])
            {
                uint32_t id = entity_json["id"].get<uint32_t>();
                bool enabled = entity_json["enabled"].get<bool>();
                std::string name = entity_json["name"].get<std::string>();

                auto position_array = entity_json["position"].get<std::vector<float>>();
                auto rotation_array = entity_json["rotation"].get<std::vector<float>>();
                auto scale_array = entity_json["scale"].get<std::vector<float>>();

                glm::vec3 position(position_array[0], position_array[1], position_array[2]);
                glm::vec3 rotation(rotation_array[0], rotation_array[1], rotation_array[2]);
                glm::vec3 scale(scale_array[0], scale_array[1], scale_array[2]);

                std::shared_ptr<Entity> entity = std::make_shared<Entity>(id, enabled, name, position, rotation,
                    scale);
                if (entity_json.contains("parent"))
                    entity->parent = entity_json["parent"].get<uint32_t>();

                const auto& entity_components = entity_json["components"];
                if (entity_components.contains("model_component"))
                {
                    uint32_t model_id = entity_components["model_component"].get<uint32_t>();
                    entity->AddComponent(ComponentType::MODEL, model_components.at(model_id));
                }
                if (entity_components.contains("light_source_component"))
                {
                    uint32_t light_id = entity_components["light_source_component"].get<uint32_t>();
                    entity->AddComponent(ComponentType::LIGHT, light_source_components.at(light_id));
                }
                if (entity_components.contains("physics_component"))
                {
                    uint32_t physics_id = entity_components["physics_component"].get<uint32_t>();
                    entity->AddComponent(ComponentType::PHYSICS, physics_components.at(physics_id));
                }
                if (entity_components.contains("animation_component"))
                {
                    uint32_t animation_id = entity_components["animation_component"].get<uint32_t>();
                    entity->AddComponent(ComponentType::ANIMATION, animation_components.at(animation_id));
                }
                if (entity_components.contains("audio_component"))
                {
                    uint32_t audio_id = entity_components["audio_component"].get<uint32_t>();
                    entity->AddComponent(ComponentType::AUDIO, audio_components.at(audio_id));
                }

                entities.insert_or_assign(id, entity);
            }

            for (auto& [id, entity] : entities)
            {
                if (entity->parent != 0 && entities.contains(entity->parent))
                {
                    entities.at(entity->parent)->AddChild(id);
                }
            }
        }

        for (auto& [id, entity] : entities)
        {
            if (entity->HasComponent<PhysicsComponent>())
            {
                PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
                std::shared_ptr<PhysicsBody> physics_body = physics_component.physics_body;

                physics_body->SetPosition(entity->position);
                physics_body->SetRotation(glm::quat(glm::radians(entity->rotation)));
            }
        }

        for (auto& [id, entity] : entities)
        {
            if (entity->HasComponent<AnimationComponent>() && entity->HasComponent<ModelComponent>())
            {
                AnimationComponent& animation_component = entity->GetComponent<AnimationComponent>();
                ModelComponent& model_component = entity->GetComponent<ModelComponent>();

                if (auto skeletal_model = std::dynamic_pointer_cast<SkeletalModel>(model_component.model))
                {
                    std::shared_ptr<Animator> animator = std::make_shared<Animator>(skeletal_model->GetSkeleton());

                    for (const auto& animation : skeletal_model->GetAnimations())
                        animator->AddAnimation(animation);

                    animation_component.animator = animator;
                }
            }
        }

        skybox = std::make_unique<Skybox>("Data/Editor/Defaults/Textures/Skyboxes/S3");
        std::shared_ptr<Shader> shadow_map_shader;
        std::shared_ptr<Shader> point_shadow_map_shader;
        std::vector<std::shared_ptr<Shader>> shadow_activated_shaders;
        for (auto& [shader_id, shader] : shaders)
        {
            if (shader->name == "Skybox")
                skybox->Load(shader);
            else if (shader->name == "Lit")
                shadow_activated_shaders.push_back(shader);
            else if (shader->name == "Shadow Map")
                shadow_map_shader = shader;
            else if (shader->name == "Point Shadow Map")
                point_shadow_map_shader = shader;
        }
        shadow_map = std::make_unique<ShadowMap>(point_shadow_map_shader, shadow_map_shader, shadow_activated_shaders, "Data/Editor/Defaults/Textures/default-diffuse.png");

        return true;
    }

}
