#include "bonfire_pch.hpp"
#include "Scene.hpp"

#include "Core/Utility.hpp"
#include "Core/Project.hpp"
#include "Physics/PhysicsSystem.hpp"

namespace Bonfire
{
    void Scene::UpdateLightSources(Shader& shader, float shininess)
    {
        shader.SetFloat("material.shininess", shininess);

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
            shader.SetVec3("point_lights[" + number + "].ambient", point_light->color / 255.0f * 0.1f);
            shader.SetVec3("point_lights[" + number + "].diffuse", point_light->color / 255.0f);
            shader.SetVec3("point_lights[" + number + "].specular", point_light->color / 255.0f);

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
        directional_light = nullptr;
        engine_camera = nullptr;
        skybox = nullptr;
        shadow_map = nullptr;

        // LOAD COMPONENT TYPES FROM PARAM DATABASE
        for (auto& [model_id, model_data] : param_database.model_params)
        {
            std::shared_ptr<Model> model = std::make_shared<Model>(model_data.path);
            model->param_id = model_id;
            model->name = model_data.name;
            model->Load();
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
            for (uint32_t tex_id : material_data.texture_ids)
            {
                if (textures.contains(tex_id))
                {
                    material->AddTexture(textures.at(tex_id));
                }
            }
            materials.insert_or_assign(material_id, std::move(material));
        }
        for (auto& [shader_id, shader_data] : param_database.shader_params)
        {
            std::shared_ptr<Shader> shader = std::make_shared<Shader>(shader_id, shader_data.name, shader_data.vert_path, shader_data.frag_path, shader_data.geom_path);
            shader->Load();
            shaders.insert_or_assign(shader_id, std::move(shader));
        }
        //  LOAD OTHER COMPONENT TYPES

        std::ifstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open scene file: " + path);
            return false;
        }

        nlohmann::json json;
        try
        {
            file >> json;
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse scene JSON: " + std::string(e.what()));
            return false;
        }

        // Load camera
        if (!json.contains("cameras"))
        {
            Log::Error("Scene file has no camera");
            return false;
        }
        for (const auto& camera_json : json["cameras"])
        {
            uint32_t id = camera_json["id"];
            float yaw = camera_json["yaw"];
            float pitch = camera_json["pitch"];
            
            auto position_array = camera_json["position"].get<std::vector<float>>();
            auto up_array = camera_json["up"].get<std::vector<float>>();
            
            glm::vec3 position(position_array[0], position_array[1], position_array[2]);
            glm::vec3 up(up_array[0], up_array[1], up_array[2]);

            engine_camera = std::make_unique<Camera>(id, position, up, yaw, pitch);
        }

        // Load components
        if (!json.contains("components"))
        {
            Log::Warning("Scene file has no components");
        }
        else
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

                    std::shared_ptr<Model> model = models.contains(model_id) ? models.at(model_id) : nullptr;
                    std::shared_ptr<Shader> shader = shaders.contains(shader_id) ? shaders.at(shader_id) : nullptr;
                    std::shared_ptr<Material> material = materials.contains(material_id) ? materials.at(material_id) : nullptr;
                    
                    std::shared_ptr<ModelComponent> model_included = std::make_shared<ModelComponent>(id, enabled, model, shader, material);
                    model_components.insert_or_assign(id, model_included);
                }
            }
            // ADD OTHER COMPONENT TYPES
            if (components.contains("light_sources"))
            {
                for (const auto& [light_component_id, light_data] : components["light_sources"].items())
                {
                    uint32_t id = std::stoul(light_component_id);
                    bool enabled = light_data["enabled"].get<bool>();
                    std::string light_type = light_data["light-type"].get<std::string>();
                    std::shared_ptr<LightSource> light_source;
                    
                    if (light_type == "point")
                    {
                        auto color_array = light_data["color"].get<std::vector<float>>();
                        auto position_array = light_data["position"].get<std::vector<float>>();
                        auto scale_array = light_data["scale"].get<std::vector<float>>();
                        glm::vec3 color(color_array[0], color_array[1], color_array[2]);
                        glm::vec3 position(position_array[0], position_array[1], position_array[2]);
                        glm::vec3 scale(scale_array[0], scale_array[1], scale_array[2]);

                        std::shared_ptr<PointLight> point_light = std::make_shared<PointLight>(color, position, scale);
                        point_light->id = light_data["light-id"].get<uint32_t>();
                        point_light->enabled = light_data["light-enabled"].get<bool>();
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
                        std::shared_ptr<LightSourceComponent> light_component = std::make_shared<LightSourceComponent>(id, enabled, light_source);
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
            
            for (auto& [id, light_component] : light_source_components)
            {
                if (auto point_light = std::dynamic_pointer_cast<PointLight>(light_component->light_source))
                    point_lights.insert_or_assign(point_light->id, point_light);
                else if (auto spot_light = std::dynamic_pointer_cast<SpotLight>(light_component->light_source))
                    spot_lights.insert_or_assign(spot_light->id, spot_light);
            }
        }

        // Load entities
        if (!json.contains("entities"))
        {
            Log::Warning("Scene file has no entities");
        }
        else
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

                std::shared_ptr<Entity> entity = std::make_shared<Entity>(id, enabled, name, position, rotation, scale);
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
        
        skybox = std::make_unique<Skybox>("S3");
        shadow_map = std::make_unique<ShadowMap>();
        std::shared_ptr<Shader> point_shadow_map_shader;
        std::shared_ptr<Shader> lit_shader;
        std::shared_ptr<Shader> shadow_map_shader;
        for (auto& [shader_id, shader] : shaders)
        {
            if (shader->name == "Skybox")
                skybox->Load(shader);
            else if (shader->name == "Point Shadow Map")
                point_shadow_map_shader = shader;
            else if (shader->name == "Lit")
                lit_shader = shader;
            else if (shader->name == "Shadow Map")
                shadow_map_shader = shader;
        }
        shadow_map->Generate(point_shadow_map_shader, shadow_map_shader, lit_shader, "Data/Resources/Textures/checkered.png");

        directional_light = std::make_unique<DirectionalLight>();

        Log::Info("Loaded scene from " + path);
        return true;
    }

    bool Scene::SaveScene(ParamDatabase& param_database)
    {
        nlohmann::json json;
        nlohmann::json camera_array = nlohmann::json::array();
        nlohmann::json entities_array = nlohmann::json::array();

        nlohmann::json camera_json;
        camera_json["id"] = engine_camera->id;
        camera_json["yaw"] = engine_camera->Yaw;
        camera_json["pitch"] = engine_camera->Pitch;
        camera_json["position"] = {engine_camera->Position.x, engine_camera->Position.y, engine_camera->Position.z};
        camera_json["up"] = {engine_camera->WorldUp.x, engine_camera->WorldUp.y, engine_camera->WorldUp.z};
        camera_array.push_back(camera_json);

        nlohmann::json components_json;
        
        // Save components
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

        // Save entities
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
        
            // Save entity components
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
        
            entity_json["components"] = entity_components_json;
            entities_array.push_back(entity_json);
        }

        json["cameras"] = camera_array;
        json["components"] = components_json;
        json["entities"] = entities_array;

        std::ofstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open scene file for writing: " + path);
            return false;
        }

        try
        {
            file << json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write scene JSON: " + std::string(e.what()));
            return false;
        }

        Log::Info("Saved scene to " + path);
        return true;
    }

}
