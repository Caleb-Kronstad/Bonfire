#pragma once

#include <miniaudio.h>

namespace Bonfire
{
    class Audio
    {
    public:
        Audio(uint32_t id, const std::string& file_path);
        ~Audio();

        void Play();
        void Pause();
        void Stop();

        void SetVolume(float volume);
        float GetVolume() const;
        void SetPitch(float pitch);
        float GetPitch() const;
        void SetLoop(bool loop);
        bool GetLoop() const;

        void Set3DPosition(glm::vec3 position);
        void Set3DMinDistance(float distance);
        void Set3DMaxDistance(float distance);
        void SetSpatialization(bool enabled);

        bool IsPlaying() const;
        bool IsPaused() const;
        bool IsEnded() const;

        std::string GetFilePath() const { return file_path; }

        bool GetPlayOnAwake() const { return play_on_awake; }
        void SetPlayOnAwake(bool play_awake) { play_on_awake = play_awake; }

    public:
        uint32_t id;
        
    private:
        std::string file_path;
        bool initialized = false;
        ma_sound sound;
        ma_engine* engine;
        bool play_on_awake = false;
    };
}
