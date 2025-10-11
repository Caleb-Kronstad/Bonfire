#pragma once


namespace Bonfire
{
    struct ModelParamData
    {
        std::string name;
        std::string path;

    }
    struct TextureParamData
    {
        std::string path;
    }

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
    }
}
