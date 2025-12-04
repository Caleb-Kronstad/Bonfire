#pragma once

#include "Audio.hpp"
#include "Core/Layer.hpp"

namespace Bonfire
{
    class AudioSystem : Layer
    {
    public:
        AudioSystem();
        ~AudioSystem();
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(const float& delta_time) override;

        bool AddAudio(std::shared_ptr<Audio> audio);
        bool RemoveAudio(std::shared_ptr<Audio> audio);
        std::shared_ptr<Audio> GetAudio(uint32_t id);
        const std::unordered_map<uint32_t, std::shared_ptr<Audio>>& GetAudios() { return audios; }

        void UpdateListener(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up);
        void UpdateAudios();
        void SetMasterVolume(float volume);
        float GetMasterVolume();
        ma_engine* GetEngine() { return &engine; }

    private:
        std::unordered_map<uint32_t, std::shared_ptr<Audio>> audios;
        ma_engine engine;
        ma_engine_config engine_config;
        bool initialized = false;
    };
}
