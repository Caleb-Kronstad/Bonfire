#include "bonfire_pch.hpp"
#include "ParamDatabase.hpp"

namespace Bonfire
{
    void ParamDatabase::LoadParams()
    {
        std::ifstream model_file(model_path);
        if (!model_file.is_open())
        {
            Log::Error("Failed to open model params file: " + model_path);
            return;
        }

        nlohmann::json model_json;
        try
        {
            model_file >> model_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse model params JSON: " + std::string(e.what()));
            return;
        }

        for (auto& [key, value] : model_json.items())
        {
            uint32_t id = std::stoul(key);
            ParamReference ref(id);

            std::string name = value["name"].get<std::string>();
            std::string path = value["path"].get<std::string>();

            auto rotation_array = value["rotationMultiplier"].get<std::vector<float>>();
            auto scale_array = value["scaleMultiplier"].get<std::vector<float>>();

            glm::vec3 rotation_multiplier(rotation_array[0], rotation_array[1], rotation_array[2]);
            glm::vec3 scale_multiplier(scale_array[0], scale_array[1], scale_array[2]);

            model_params[ref] = ModelParamData(name, path, rotation_multiplier, scale_multiplier);
        }
        Log::Info("Loaded " + std::to_string(model_params.size()) + " model params from " + model_path);

        std::ifstream texture_file(texture_path);
        if (!texture_file.is_open())
        {
            Log::Error("Failed to open texture params file: " + texture_path);
            return;
        }

        nlohmann::json texture_json;
        try
        {
            texture_file >> texture_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse texture params JSON: " + std::string(e.what()));
            return;
        }

        for (auto& [key,value] : texture_json.items())
        {
            uint32_t id = std::stoul(key);
            ParamReference ref(id);

            TEXTURE_TYPE type = value["type"].get<TEXTURE_TYPE>();
            bool flip = value["flip"].get<bool>();
            std::string path = value["path"].get<std::string>();
            texture_params[ref] = TextureParamData(type, flip, path);
        }

        Log::Info("Loaded " + std::to_string(texture_params.size()) + " texture params from " + texture_path);

        // load other param types
    }

    void ParamDatabase::SaveParams()
    {
        nlohmann::json model_json;

        for (const auto& [ref, data] : model_params)
        {
            std::string key = std::to_string(ref.value);
            model_json[key] = {
                {"name", data.name},
                {"path", data.path},
                {"rotationMultiplier", {data.rotation_multiplier.x, data.rotation_multiplier.y, data.rotation_multiplier.z}},
                {"scaleMultiplier", {data.scale_multiplier.x, data.scale_multiplier.y, data.scale_multiplier.z}}
            };
        }

        std::ofstream model_file(model_path);
        if (!model_file.is_open())
        {
            Log::Error("Failed to open model params file for writing: " + model_path);
            return;
        }
        try
        {
            model_file << model_json.dump(4); // 4 spaces for indentation
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write model params JSON: " + std::string(e.what()));
            return;
        }

        Log::Info("Saved " + std::to_string(model_params.size()) + " model params to " + model_path);

        nlohmann::json texture_json;

        for (const auto& [ref, data] : texture_params)
        {
            std::string key = std::to_string(ref.value);
            texture_json[key] = {
                {"type", data.type},
                {"flip", data.flip},
                {"path", data.path}
            };
        }

        std::ofstream texture_file(texture_path);
        if (!texture_file.is_open())
        {
            Log::Error("Failed to open texture params file for writing: " + texture_path);
            return;
        }

        try
        {
            texture_file << texture_json.dump(4); // 4 spaces for indentation
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write texture params JSON: " + std::string(e.what()));
            return;
        }

        Log::Info("Saved " + std::to_string(texture_params.size()) + " texture params to " + texture_path);

        // save other param types
    }
}
