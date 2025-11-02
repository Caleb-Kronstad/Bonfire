#pragma once

namespace Bonfire
{
    class Material;
    
    struct ModelParamData
    {
        std::string name;
        std::string path;
        bool is_animated;

        ModelParamData() { }
        ModelParamData(const std::string& name, const std::string& path, const bool& is_animated) : name(name), path(path), is_animated(is_animated) {}
    };
    struct TextureParamData
    {
        std::string name;
        TextureType type;
        bool flip;
        std::string path;

        TextureParamData() { }
        TextureParamData(const std::string& name, const TextureType& type, const bool& flip, const std::string& path) : name(name), type(type), flip(flip), path(path) {}
    };
    struct MaterialParamData
    {
        std::string name;
        uint32_t diffuse_id;
        uint32_t specular_id;
        uint32_t normal_id;
        uint32_t height_id;
        uint32_t emission_id;
        float shininess;
        glm::vec2 tiling;
        glm::vec2 offset;

        MaterialParamData() { }
        MaterialParamData(const std::string& name,
            const uint32_t& diffuse_id, const uint32_t& specular_id, const uint32_t& normal_id, const uint32_t& height_d, const uint32_t& emission_id,
            const float& shininess, const glm::vec2& tiling, const glm::vec2& offset)
            : name(name),
        diffuse_id(diffuse_id), specular_id(specular_id), normal_id(normal_id), height_id(height_d), emission_id(emission_id),
        shininess(shininess), tiling(tiling), offset(offset) {}
    };
    struct ShaderParamData
    {
        std::string name;
        std::string vert_path;
        std::string frag_path;
        std::string geom_path;

        ShaderParamData() { }
        ShaderParamData(const std::string& name, const std::string& vert_path, const std::string& frag_path, const std::string& geom_path) : name(name), vert_path(vert_path), frag_path(frag_path), geom_path(geom_path) {}
    };
    struct AudioParamData
    {
        std::string name;
        std::string path;

        AudioParamData() { }
        AudioParamData(const std::string& name, const std::string& path) : name(name), path(path) {}
    };

    class ParamDatabase
    {
    public:
        ParamDatabase(const std::string& model_path = "", const std::string& texture_path = "", const std::string& shader_path = "", const std::string& material_path = "", const std::string& audio_path = "")
            : model_path(model_path), texture_path(texture_path), shader_path(shader_path), material_path(material_path), audio_path(audio_path)
        {
        }
        
        bool LoadParams();
        bool SaveParams(const std::unordered_map<uint32_t, std::shared_ptr<Material>>& materials);

        ModelParamData& GetModelParam(uint32_t id) { return model_params.at(id); }
        TextureParamData& GetTextureParam(uint32_t id) { return texture_params.at(id); }
        ShaderParamData& GetShaderParam(uint32_t id) { return shader_params.at(id); }
        MaterialParamData& GetMaterialParam(uint32_t id) { return material_params.at(id); }
        AudioParamData& GetAudioParam(uint32_t id) { return audio_params.at(id); }

    public:
        std::unordered_map<uint32_t, ModelParamData> model_params;
        std::unordered_map<uint32_t, TextureParamData> texture_params;
        std::unordered_map<uint32_t, ShaderParamData> shader_params;
        std::unordered_map<uint32_t, MaterialParamData> material_params;
        std::unordered_map<uint32_t, AudioParamData> audio_params;

    private:
        std::string model_path;
        std::string texture_path;
        std::string shader_path;
        std::string material_path;
        std::string audio_path;
    };
}