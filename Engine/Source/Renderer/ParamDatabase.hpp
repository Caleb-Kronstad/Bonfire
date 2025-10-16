#pragma once

namespace Bonfire
{
    
    struct ModelParamData
    {
        std::string name;
        std::string path;

        ModelParamData() { name = ""; path = "";}
        ModelParamData(const std::string& name, const std::string& path) : name(name), path(path) {}
    };
    struct TextureParamData
    {
        std::string name;
        TEXTURE_TYPE type;
        bool flip;
        std::string path;

        TextureParamData() { name = ""; type = TEXTURE_TYPE::DIFFUSE; flip = false; path = ""; }
        TextureParamData(const std::string& name, const TEXTURE_TYPE& type, const bool& flip, const std::string& path) : name(name), type(type), flip(flip), path(path) {}
    };

    class ParamDatabase
    {
    public:
        ParamDatabase(const std::string& model_path = "", const std::string& texture_path = "")
            : model_path(model_path), texture_path(texture_path)
        {
        }
        
        void LoadParams();
        void SaveParams();

        ModelParamData& GetModelParam(uint32_t ref) { return model_params.at(ref); }
        TextureParamData& GetTextureParam(uint32_t ref) { return texture_params.at(ref); }

    public:
        std::unordered_map<uint32_t, ModelParamData> model_params;
        std::unordered_map<uint32_t, TextureParamData> texture_params;

    private:
        std::string model_path;
        std::string texture_path;
    };
}