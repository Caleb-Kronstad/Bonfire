#include "bonfire_pch.hpp"
#include "Scene.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
    bool Scene::LoadScene(ParamDatabase& param_database)
    {
        entities.clear();
        models.clear();
        textures.clear();
        shaders.clear();
        model_components.clear();
        texture_components.clear();

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
                for (const auto& [model_id, model_data] : components["models"].items())
                {
                    uint32_t id = std::stoul(model_id);
                    bool enabled = model_data["enabled"].get<bool>();
                    uint32_t param_id = model_data["param-id"].get<uint32_t>();
                    uint32_t shader_id = model_data["shader-id"].get<uint32_t>();
                    std::shared_ptr<ModelComponent> model_included = std::make_shared<ModelComponent>(id, enabled, models.at(param_id), shaders.at(shader_id));
                    model_components.insert_or_assign(id, model_included);
                }
            }
            if (components.contains("textures"))
            {
                for (const auto& [texture_id, texture_data] : components["textures"].items())
                {
                    uint32_t id = std::stoul(texture_id);
                    bool enabled = texture_data["enabled"].get<bool>();
                    std::vector<uint32_t> param_ids = texture_data["param-ids"].get<std::vector<uint32_t>>();
                    std::vector<std::shared_ptr<Texture>> textures_included;
                    for (const auto& param_id : param_ids)
                        textures_included.push_back(textures.at(param_id));
                    std::shared_ptr<TextureComponent> texture_included = std::make_shared<TextureComponent>(id, enabled, textures_included);
                    texture_components.insert_or_assign(id, texture_included);
                }
            }
            // ADD OTHER COMPONENT TYPES
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
                    entity->AddComponent(COMPONENT_TYPE::MODEL, model_components.at(model_id));
                }
                if (entity_components.contains("texture_component"))
                {
                    uint32_t model_id = entity_components["texture_component"].get<uint32_t>();
                    entity->AddComponent(COMPONENT_TYPE::TEXTURE, texture_components.at(model_id));
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
        
        // Save model components
        nlohmann::json models_json;
        for (const auto& [id, model_component] : model_components)
        {
            nlohmann::json model_json;
            model_json["enabled"] = model_component->enabled;
            model_json["param-id"] = model_component->model->param_id;
            model_json["shader-id"] = model_component->shader->param_id;
            models_json[std::to_string(id)] = model_json;
        }
        if (!models_json.empty())
            components_json["models"] = models_json;
        
        nlohmann::json textures_json;
        for (const auto& [id, texture_component] : texture_components)
        {
            nlohmann::json texture_json;
            texture_json["enabled"] = texture_component->enabled;
        
            std::vector<uint32_t> param_ids;
            for (const auto& texture : texture_component->textures)
                param_ids.push_back(texture->param_id);
            texture_json["param-ids"] = param_ids;
        
            textures_json[std::to_string(id)] = texture_json;
        }
        if (!textures_json.empty())
            components_json["textures"] = textures_json;

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
                auto& model_component = entity->GetComponent<ModelComponent>();
                entity_components_json["model_component"] = model_component.id;
            }

            if (entity->HasComponent<TextureComponent>())
            {
                auto& texture_component = entity->GetComponent<TextureComponent>();
                entity_components_json["texture_component"] = texture_component.id;
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
