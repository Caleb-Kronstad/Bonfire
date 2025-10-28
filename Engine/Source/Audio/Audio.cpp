#include "bonfire_pch.hpp"
#define MINIAUDIO_IMPLEMENTATION
#include "Audio.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
    Audio::Audio(uint32_t id, const std::string& file_path)
        : id(id), file_path(file_path)
    {
        engine = Project::GetAudioSystem().GetEngine();
        
        if (engine == nullptr)
        {
            Log::Error("Audio engine not found, cannot create Audio");
            return;
        }

        ma_result result = ma_sound_init_from_file(engine, file_path.c_str(), MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, NULL, NULL, &sound);
        if (result != MA_SUCCESS)
        {
            Log::Error("Failed to load audio file " + file_path);
            initialized = false;
            return;
        }

        initialized = true;
    }

    Audio::~Audio()
    {
        if (initialized)
            ma_sound_uninit(&sound);
    }

    void Audio::Play()
    {
        if (!initialized) return;
        ma_sound_start(&sound);
    }

    void Audio::Pause()
    {
        if (!initialized) return;
        ma_sound_stop(&sound);
    }

    void Audio::Stop()
    {
        if (!initialized) return;
        ma_sound_stop(&sound);
    }

    void Audio::SetVolume(float volume)
    {
        if (!initialized) return;
        ma_sound_set_volume(&sound, glm::clamp(volume, 0.0f, 1.0f));
    }

    float Audio::GetVolume() const
    {
        if (!initialized) return 0.0f;
        return ma_sound_get_volume(&sound);
    }

    void Audio::SetPitch(float pitch)
    {
        if (!initialized) return;
        ma_sound_set_pitch(&sound, pitch);
    }

    float Audio::GetPitch() const
    {
        if (!initialized) return 0.0f;
        return ma_sound_get_pitch(&sound);
    }
    
    void Audio::SetLoop(bool loop)
    {
        if (!initialized) return;
        ma_sound_set_looping(&sound, loop ? MA_TRUE : MA_FALSE);
    }

    bool Audio::GetLoop() const
    {
        if (!initialized) return false;
        return ma_sound_is_looping(&sound) == MA_TRUE;
    }

    void Audio::Set3DPosition(glm::vec3 position)
    {
        if (!initialized) return;
        ma_sound_set_position(&sound, position.x, position.y, position.z);
    }

    void Audio::Set3DMinDistance(float distance)
    {
        if (!initialized) return;
        ma_sound_set_min_distance(&sound, distance);
    }

    void Audio::Set3DMaxDistance(float distance)
    {
        if (!initialized) return;
        ma_sound_set_max_distance(&sound, distance);
    }

    void Audio::SetSpatialization(bool enabled)
    {
        if (!initialized) return;
        ma_sound_set_spatialization_enabled(&sound, enabled ? MA_TRUE : MA_FALSE);
    }

    bool Audio::IsPlaying() const
    {
        if (!initialized) return false;
        return ma_sound_is_playing(&sound) == MA_TRUE;
    }

    bool Audio::IsPaused() const
    {
        if (!initialized) return false;
        return !IsPlaying() && !IsEnded();
    }

    bool Audio::IsEnded() const
    {
        if (!initialized) return false;
        return ma_sound_at_end(&sound) == MA_TRUE;
    }

}
