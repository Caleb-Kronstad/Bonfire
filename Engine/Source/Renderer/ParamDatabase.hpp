#pragma once

namespace Bonfire
{
    class Material;
    
    struct ModelParamData
    {
        std::string name;
        std::string path;
        bool is_animated;

        ModelParamData() { name = ""; path = "";}
        ModelParamData(const std::string& name, const std::string& path, const bool& is_animated) : name(name), path(path), is_animated(is_animated) {}
    };
    struct TextureParamData
    {
        std::string name;
        TextureType type;
        bool flip;
        std::string path;

        TextureParamData() { name = "Texture"; type = TextureType::DIFFUSE; flip = false; path = ""; }
        TextureParamData(const std::string& name, const TextureType& type, const bool& flip, const std::string& path) : name(name), type(type), flip(flip), path(path) {}
    };
    struct MaterialParamData
    {
        std::string name;
        std::vector<uint32_t> texture_ids;

        MaterialParamData() { name = ""; }
        MaterialParamData(const std::string& name, const std::vector<uint32_t>& texture_ids) : name(name), texture_ids(texture_ids) {}
    };
    struct ShaderParamData
    {
        std::string name;
        std::string vert_path;
        std::string frag_path;
        std::string geom_path;

        ShaderParamData() { name = "Shader"; vert_path = ""; frag_path = ""; geom_path = ""; }
        ShaderParamData(const std::string& name, const std::string& vert_path, const std::string& frag_path, const std::string& geom_path) : name(name), vert_path(vert_path), frag_path(frag_path), geom_path(geom_path) {}
    };
    struct AudioParamData
    {
        std::string name;
        std::string path;

        AudioParamData() { name = "Audio"; path = ""; }
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