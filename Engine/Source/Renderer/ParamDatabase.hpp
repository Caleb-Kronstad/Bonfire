#pragma once

namespace Bonfire
{
    struct ParamReference {
        uint32_t value;

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

        ModelParamData(const std::string& name, const std::string& path, const glm::vec3& rotation_multiplier, const glm::vec3& scale_multiplier) : name(name), path(path), rotation_multiplier(rotation_multiplier), scale_multiplier(scale_multiplier) {}
    };
    struct TextureParamData
    {
        TEXTURE_TYPE type;
        bool flip;
        std::string path;

        TextureParamData(const TEXTURE_TYPE& type, const bool& flip, const std::string& path) : type(type), flip(flip), path(path) {}
    };
    struct AIParamData
    {
        
    };

    class ParamDatabase
    {
    public:
        void LoadModelParams(const std::string& filepath);
        void LoadTextureParams(const std::string& filepath);
        void LoadAIParams(const std::string& filepath);

        ModelParamData GetModelParam(ParamReference ref) { return model_params.at(ref); }
        TextureParamData GetTextureParam(ParamReference ref) { return texture_params.at(ref); }
        AIParamData GetAIParam(ParamReference ref) { return ai_params.at(ref); }

    private:
        std::unordered_map<ParamReference, ModelParamData> model_params;
        std::unordered_map<ParamReference, TextureParamData> texture_params;
        std::unordered_map<ParamReference, AIParamData> ai_params;
    };
}