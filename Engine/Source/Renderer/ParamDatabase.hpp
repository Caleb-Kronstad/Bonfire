#pragma once

namespace Bonfire
{
    struct ParamReference {
        uint32_t value;

        ParamReference() : value(0) {}
        explicit ParamReference(uint32_t v) : value(v) {}

        bool operator==(const ParamReference& other) const { return value == other.value; }
        bool operator!=(const ParamReference& other) const { return value != other.value; }
    };
}

namespace std
{
    
    template<>
    struct hash<Bonfire::ParamReference>
    {
        size_t operator()(const Bonfire::ParamReference& ref) const
        {
            return hash<uint32_t>()(ref.value);
        }
    };
}

namespace Bonfire
{
    
    struct ModelParamData
    {
        std::string name;
        std::string path;
        glm::vec3 rotation_multiplier;
        glm::vec3 scale_multiplier;

        ModelParamData() { name = ""; path = ""; rotation_multiplier = glm::vec3(1.0f); glm::vec3 scale_multiplier = glm::vec3(1.0f); }
        ModelParamData(const std::string& name, const std::string& path, const glm::vec3& rotation_multiplier, const glm::vec3& scale_multiplier) : name(name), path(path), rotation_multiplier(rotation_multiplier), scale_multiplier(scale_multiplier) {}
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
    struct AIParamData
    {
        AIParamData() {}
    };

    class ParamDatabase
    {
    public:
        ParamDatabase(const std::string& model_path = "", const std::string& texture_path = "", const std::string& ai_path = "")
            : model_path(model_path), texture_path(texture_path), ai_path(ai_path)
        {
        }
        
        void LoadParams();
        void SaveParams();

        ModelParamData GetModelParam(ParamReference ref) { return model_params.at(ref); }
        TextureParamData GetTextureParam(ParamReference ref) { return texture_params.at(ref); }
        AIParamData GetAIParam(ParamReference ref) { return ai_params.at(ref); }

    public:
        std::unordered_map<ParamReference, ModelParamData> model_params;
        std::unordered_map<ParamReference, TextureParamData> texture_params;
        std::unordered_map<ParamReference, AIParamData> ai_params;

    private:
        std::string model_path;
        std::string texture_path;
        std::string ai_path;
    };
}