#include "bonfire_pch.hpp"
#include "ParamDatabase.hpp"

namespace Bonfire
{
    bool ParamDatabase::LoadParams()
    {
        std::ifstream model_file(model_path);
        if (!model_file.is_open())
        {
            Log::Error("Failed to open model params file: " + model_path);
            return false;
        }

        nlohmann::json model_json;
        try
        {
            model_file >> model_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse model params JSON: " + std::string(e.what()));
            return false;
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
            return false;
        }

        nlohmann::json texture_json;
        try
        {
            texture_file >> texture_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse texture params JSON: " + std::string(e.what()));
            return false;
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

        std::ifstream shader_file(shader_path);
        if (!shader_file.is_open())
        {
            Log::Error("Failed to open shader params file: " + shader_path);
            return false;
        }

        nlohmann::json shader_json;
        try
        {
            shader_file >> shader_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse shader params JSON: " + std::string(e.what()));
            return false;
        }

        for (auto& [key, value] : shader_json.items())
        {
            uint32_t id = std::stoul(key);
            uint32_t ref(id);

            std::string name = value["name"].get<std::string>();
            std::string vert_path = value["vert-path"].get<std::string>();
            std::string frag_path = value["frag-path"].get<std::string>();
            std::string geom_path = value["geom-path"].get<std::string>();
            shader_params[ref] = ShaderParamData(name, vert_path, frag_path, geom_path);
        }

        // load other param types

        Log::Info("Successfully loaded params");
        return true;
    }

    bool ParamDatabase::SaveParams()
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
            return false;
        }
        try
        {
            model_file << model_json.dump(4); // 4 spaces for indentation
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write model params JSON: " + std::string(e.what()));
            return false;
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
            return false;
        }
        try
        {
            texture_file << texture_json.dump(4); // 4 spaces for indentation
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write texture params JSON: " + std::string(e.what()));
            return false;
        }

        nlohmann::json shader_json;

        for (const auto& [ref, data] : shader_params)
        {
            std::string key = std::to_string(ref);
            shader_json[key] = {
                {"name", data.name},
                {"vert-path", data.vert_path},
                {"frag-path", data.frag_path},
                {"geom-path", data.geom_path}
            };
        }

        std::ofstream shader_file(shader_path);
        if (!shader_file.is_open())
        {
            Log::Error("Failed to open shader params file for writing: " + shader_path);
            return false;
        }
        try
        {
            shader_file << shader_json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write shader params JSON: " + std::string(e.what()));
            return false;
        }

        // save other param types

        Log::Info("Successfully saved params");
        return true;
    }
}
