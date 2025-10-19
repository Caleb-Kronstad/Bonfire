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
            uint32_t ref(id);

            std::string name = value["name"].get<std::string>();
            std::string path = value["path"].get<std::string>();

            model_params[ref] = ModelParamData(name, path);
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
            uint32_t ref(id);

            std::string name = value["name"].get<std::string>();
            TEXTURE_TYPE type = value["type"].get<TEXTURE_TYPE>();
            bool flip = value["flip"].get<bool>();
            std::string path = value["path"].get<std::string>();
            texture_params[ref] = TextureParamData(name, type, flip, path);
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

            if (value.contains("texture_ids"))
            {
                for (auto& tex_id : value["texture_ids"])
                {
                    material_data.texture_ids.push_back(tex_id.get<uint32_t>());
                }
            }

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
            uint32_t ref(id);

            std::string name = value["name"].get<std::string>();
            std::string vert_path = value["vert-path"].get<std::string>();
            std::string frag_path = value["frag-path"].get<std::string>();
            std::string geom_path = value["geom-path"].get<std::string>();
            shader_params[ref] = ShaderParamData(name, vert_path, frag_path, geom_path);
        }
        shader_file.close();

        // load other param types

        Log::Info("Successfully loaded params");
        return true;
    }

    bool ParamDatabase::SaveParams(const std::unordered_map<uint32_t, std::shared_ptr<Material>>& materials)
    {
        // model params
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

        // material params
        nlohmann::json material_json;
        for (auto& [material_id, material_obj] : materials)
        {
            if (material_params.contains(material_id))
            {
                material_params[material_id].name = material_obj->name;
                material_params[material_id].texture_ids.clear();

                for (const auto& texture : material_obj->GetTextures())
                    material_params[material_id].texture_ids.push_back(texture->param_id);
            }
            else
            {
                std::vector<uint32_t> texture_ids;
                for (const auto& texture : material_obj->GetTextures())
                    texture_ids.push_back(texture->param_id);
                
                material_params[material_id] = MaterialParamData(material_obj->name, texture_ids);
            }
        }

        for (auto& [id, material_data] : material_params)
        {
            material_json[std::to_string(id)] = {
                {"name", material_data.name},
                {"texture_ids", material_data.texture_ids}
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

        // save other param types

        Log::Info("Successfully saved params");
        return true;
    }
}
