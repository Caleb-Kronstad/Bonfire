#include "bonfire_pch.hpp"
#include "ParamDatabase.hpp"

#include "Material.hpp"

namespace Bonfire
{
    bool ParamDatabase::LoadParams()
    {
        // model params
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

            std::string name = value["name"].get<std::string>();
            std::string path = value["path"].get<std::string>();
            bool is_animated = value["is_animated"].get<bool>();

            model_params[id] = ModelParamData(name, path, is_animated);
        }
        model_file.close();

        // texture params
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

            std::string name = value["name"].get<std::string>();
            TextureType type = value["type"].get<TextureType>();
            bool flip = value["flip"].get<bool>();
            std::string path = value["path"].get<std::string>();
            texture_params[id] = TextureParamData(name, type, flip, path);
        }
        texture_file.close();

        // material params
        std::ifstream material_file(material_path);
        if (!material_file.is_open())
        {
            Log::Error("Failed to open material params file: " + material_path);
            return false;
        }
        nlohmann::json material_json;
        try
        {
            material_file >> material_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse material params JSON: " + std::string(e.what()));
            return false;
        }

        for (auto& [key, value] : material_json.items())
        {
            uint32_t id = std::stoul(key);
            MaterialParamData material_data;
            material_data.name = value["name"].get<std::string>();
            material_data.diffuse_id = value["diffuse-id"].get<uint32_t>();
            material_data.specular_id = value["specular-id"].get<uint32_t>();
            material_data.normal_id = value["normal-id"].get<uint32_t>();
            material_data.height_id = value["height-id"].get<uint32_t>();
            material_data.emission_id = value["emission-id"].get<uint32_t>();
            material_data.shininess = value["shininess"].get<float>();
            std::vector<float> tiling_array = value["tiling"].get<std::vector<float>>();
            std::vector<float> offset_array = value["offset"].get<std::vector<float>>();
            material_data.tiling = glm::vec2(tiling_array[0], tiling_array[1]);
            material_data.offset = glm::vec2(offset_array[0], offset_array[1]);

            material_params[id] = material_data;
        }
        material_file.close();
        
        // shader params
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

            std::string name = value["name"].get<std::string>();
            std::string vert_path = value["vert-path"].get<std::string>();
            std::string frag_path = value["frag-path"].get<std::string>();
            std::string geom_path = value["geom-path"].get<std::string>();
            shader_params[id] = ShaderParamData(name, vert_path, frag_path, geom_path);
        }
        shader_file.close();

        // audio params
        std::ifstream audio_file(audio_path);
        if (!audio_file.is_open())
        {
            Log::Error("Failed to open audio params file: " + audio_path);
            return false;
        }

        nlohmann::json audio_json;
        try
        {
            audio_file >> audio_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse audio params JSON: " + std::string(e.what()));
            return false;
        }

        for (auto& [key, value] : audio_json.items())
        {
            uint32_t id = std::stoul(key);

            std::string name = value["name"].get<std::string>();
            std::string path = value["path"].get<std::string>();
            audio_params[id] = AudioParamData(name, path);
        }
        audio_file.close();

        // script params
        std::ifstream script_file(script_path);
        if (!script_file.is_open())
        {
            Log::Error("Failed to open script params file: " + script_path);
            return false;
        }

        nlohmann::json script_json;
        try
        {
            script_file >> script_json;
        } catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to parse script params JSON: " + std::string(e.what()));
            return false;
        }

        for (auto& [key, value] : script_json.items())
        {
            uint32_t id = std::stoul(key);

            std::string name = value["name"].get<std::string>();
            std::string path = value["path"].get<std::string>();
            script_params[id] = ScriptParamData(name, path);
        }
        script_file.close();

        // load other param types

        Log::Info("Successfully loaded params");
        return true;
    }

    bool ParamDatabase::SaveParams(const std::unordered_map<uint32_t, std::shared_ptr<Material>>& materials)
    {
        // model params
        nlohmann::json model_json;

        for (const auto& [id, data] : model_params)
        {
            std::string key = std::to_string(id);
            model_json[key] = {
                {"name", data.name},
                {"path", data.path},
                {"is_animated", data.is_animated}
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
        model_file.close();

        // texture params
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
        texture_file.close();

        // material params
        nlohmann::json material_json;
        for (auto& [material_id, material] : materials)
        {
            material_params[material_id].name = material->name;
            material_params[material_id].diffuse_id = material->GetTexture(TextureType::DIFFUSE)->param_id;
            material_params[material_id].specular_id = material->GetTexture(TextureType::SPECULAR)->param_id;
            material_params[material_id].normal_id = material->GetTexture(TextureType::NORMAL)->param_id;
            material_params[material_id].height_id = material->GetTexture(TextureType::HEIGHT)->param_id;
            material_params[material_id].emission_id = material->GetTexture(TextureType::EMISSION)->param_id;
            material_params[material_id].shininess = material->shininess;
            material_params[material_id].tiling = material->texture_tiling;
            material_params[material_id].offset = material->texture_offset;
        }
        
        for (auto& [id, material_data] : material_params)
        {
            material_json[std::to_string(id)] = {
                {"name", material_data.name},
                {"diffuse-id", material_data.diffuse_id},
                {"specular-id", material_data.specular_id},
                {"normal-id", material_data.normal_id},
                {"height-id", material_data.height_id},
                {"emission-id", material_data.emission_id},
                {"shininess", material_data.shininess},
                {"tiling", {material_data.tiling[0], material_data.tiling[1]}},
                {"offset", {material_data.offset[0], material_data.offset[1]}}
            };
        }
        
        std::ofstream material_file(material_path);
        if (!material_file.is_open())
        {
            Log::Error("Failed to open material params file for writing: " + material_path);
            return false;
        }
        try
        {
            material_file << material_json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write material params JSON: " + std::string(e.what()));
            return false;
        }
        material_file.close();

        // shader params
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
        shader_file.close();

        // audio params
        nlohmann::json audio_json;

        for (const auto& [ref, data] : audio_params)
        {
            std::string key = std::to_string(ref);
            audio_json[key] = {
                {"name", data.name},
                {"path", data.path}
            };
        }

        std::ofstream audio_file(audio_path);
        if (!audio_file.is_open())
        {
            Log::Error("Failed to open audio params file for writing: " + audio_path);
            return false;
        }
        try
        {
            audio_file << audio_json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write audio params JSON: " + std::string(e.what()));
            return false;
        }
        audio_file.close();

        // script params
        nlohmann::json script_json;

        for (const auto& [ref, data] : script_params)
        {
            std::string key = std::to_string(ref);
            script_json[key] = {
                {"name", data.name},
                {"path", data.path}
            };
        }

        std::ofstream script_file(script_path);
        if (!script_file.is_open())
        {
            Log::Error("Failed to open script params file for writing: " + script_path);
            return false;
        }
        try
        {
            script_file << script_json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Log::Error("Failed to write script params JSON: " + std::string(e.what()));
            return false;
        }
        script_file.close();

        // save other param types

        Log::Info("Successfully saved params");
        return true;
    }
}
