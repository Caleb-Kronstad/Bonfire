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
            uint32_t ref(id);

            std::string name = value["name"].get<std::string>();
            std::string path = value["path"].get<std::string>();

            model_params[ref] = ModelParamData(name, path);
        }

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
            uint32_t ref(id);

            std::string name = value["name"].get<std::string>();
            TEXTURE_TYPE type = value["type"].get<TEXTURE_TYPE>();
            bool flip = value["flip"].get<bool>();
            std::string path = value["path"].get<std::string>();
            texture_params[ref] = TextureParamData(name, type, flip, path);
        }

        // load other param types
    }

    void ParamDatabase::SaveParams()
    {
        nlohmann::json model_json;

        for (const auto& [ref, data] : model_params)
        {
            std::string key = std::to_string(ref);
            model_json[key] = {
                {"name", data.name},
                {"path", data.path}
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

        nlohmann::json texture_json;

        for (const auto& [ref, data] : texture_params)
        {
            std::string key = std::to_string(ref);
            texture_json[key] = {
                {"name", data.name},
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

        // save other param types
    }
}
