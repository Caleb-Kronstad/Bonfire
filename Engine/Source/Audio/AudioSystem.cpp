#include "bonfire_pch.hpp"
#include "AudioSystem.hpp"

namespace Bonfire
{
    AudioSystem::AudioSystem()
    {
    }
    AudioSystem::~AudioSystem()
    {
        if (initialized)
            ma_engine_uninit(&engine);
    }

    void AudioSystem::OnAttach()
    {
        engine_config = ma_engine_config_init();
        ma_result result = ma_engine_init(&engine_config, &engine);
        if (result != MA_SUCCESS)
        {
            Log::Error("Failed to initialize audio engine");
            initialized = false;
            return;
        }

        initialized = true;
    }

    void AudioSystem::OnDetach()
    {
        audios.clear();
        if (initialized)
        {
            ma_engine_uninit(&engine);
            initialized = false;
        }
    }

    void AudioSystem::OnUpdate(const float& deltaTime)
    {
        
    }

    bool AudioSystem::AddAudio(std::shared_ptr<Audio> audio)
    {
        if (!initialized)
        {
            Log::Error("Audio system not initialized, cannot add audio");
            return false;
        }
        if (audios.contains(audio->id))
        {
            //Log::Error("Audio with id " + std::to_string(audio->id) + " already exists");
            return false;
        }
        audios.insert_or_assign(audio->id, audio);
        return true;
    }
    bool AudioSystem::RemoveAudio(std::shared_ptr<Audio> audio)
    {
        if (audios.contains(audio->id))
        {
            audios.erase(audio->id);
            return true;
        }
        Log::Warning("Audio with name " + audio->name + " , id: " + std::to_string(audio->id) + " does not exist");
        return false;
    }
    std::shared_ptr<Audio> AudioSystem::GetAudio(uint32_t id)
    {
        if (audios.contains(id))
            return audios.at(id);

        Log::Warning("Audio with id " + std::to_string(id) + " does not exist");
        return nullptr;
    }

    void AudioSystem::UpdateListener(const glm::vec3& position, const glm::vec3& forward, const glm::vec3& up)
    {
        if (!initialized) return;
        ma_engine_listener_set_position(&engine, 0, position.x, position.y, position.z);
        ma_engine_listener_set_direction(&engine, 0, forward.x, forward.y, forward.z);
        ma_engine_listener_set_world_up(&engine, 0, up.x, up.y, up.z);
    }

    void AudioSystem::SetMasterVolume(float volume)
    {
        if (!initialized) return;
        ma_engine_set_volume(&engine, glm::clamp(volume, 0.0f, 1.0f));
    }

    float AudioSystem::GetMasterVolume()
    {
        if (!initialized) return 0.0f;
        return ma_engine_get_volume(&engine);
    }
    
    
}
