#include "bonfire_pch.hpp"
#include "ParamDatabase.hpp"

namespace Bonfire
{
    void ParamDatabase::LoadModelParams(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            Log::Error("Failed to open model params file: " + filepath);
            return;
        }

        nlohmann::json json;
        try
        {
            file >> json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse model params JSON: " + std::string(e.what()));
            return;
        }

        for (auto& [key, value] : json.items())
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
        Log::Info("Loaded " + std::to_string(model_params.size()) + " model params from " + filepath);
    }
    void ParamDatabase::LoadTextureParams(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            Log::Error("Failed to open texture params file: " + filepath);
            return;
        }

        nlohmann::json json;
        try
        {
            file >> json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse texture params JSON: " + std::string(e.what()));
            return;
        }

        for (auto& [key,value] : json.items())
        {
            uint32_t id = std::stoul(key);
            ParamReference ref(id);

            TEXTURE_TYPE type = value["type"].get<TEXTURE_TYPE>();
            bool flip = value["flip"].get<bool>();
            std::string path = value["path"].get<std::string>();
            texture_params[ref] = TextureParamData(type, flip, path);
        }

        Log::Info("Loaded " + std::to_string(texture_params.size()) + " texture params from " + filepath);
    }
    void ParamDatabase::LoadAIParams(const std::string& filepath)
    {
        Log::Warning("LoadAIParams not yet implemented");
    }
}
