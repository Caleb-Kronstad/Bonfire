#include "bonfire_pch.hpp"
#include "Scene.hpp"

#include "Core/Utility.hpp"

namespace Bonfire
{
    bool Scene::Load()
    {
        entities.clear();
        entities_data.clear();

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

        // Load entities
        if (!json.contains("entities"))
        {
            Log::Warning("Scene file has no entities array");
            return false;
        }
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

            EntityID entity_id(id);
            EntityData entity_data(enabled, name, position, rotation, scale);

            if (entity_json.contains("params"))
            {
                for (auto& [key, value] : entity_json["params"].items())
                {
                    PARAM_TYPE param_type = StringToParamType(key);
                    uint32_t param_value = value.get<uint32_t>();
                    entity_data.AddParam(param_type, ParamReference(param_value));
                }
            }

            entities.push_back(entity_id);
            entities_data.insert_or_assign(entity_id, entity_data);
        }

        Log::Info("Loaded scene from " + path);
        return true;
    }

    bool Scene::Save()
    {
        nlohmann::json json;
        nlohmann::json entities_array = nlohmann::json::array();

        for (const auto& entity_id : entities)
        {
            const auto& entity_data = entities_data[entity_id];
        
            nlohmann::json entity_json;
            entity_json["id"] = entity_id.value;
            entity_json["enabled"] = entity_data.enabled;
            entity_json["name"] = entity_data.name;
        
            entity_json["position"] = {entity_data.position.x, entity_data.position.y, entity_data.position.z};
            entity_json["rotation"] = {entity_data.rotation.x, entity_data.rotation.y, entity_data.rotation.z};
            entity_json["scale"] = {entity_data.scale.x, entity_data.scale.y, entity_data.scale.z};
        
            if (!entity_data.params.empty())
            {
                nlohmann::json params_json;
                for (const auto& [param_type, param_ref] : entity_data.params)
                {
                    std::string key = ParamTypeToString(param_type);
                    params_json[key] = param_ref.value;
                }
                entity_json["params"] = params_json;
            }
        
            entities_array.push_back(entity_json);
        }

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
