#pragma once

#include "Animation/AnimationTrack.hpp"

namespace Bonfire
{
    class Animation
    {
    public:
        Animation(const std::string& name, float duration, float ticks_per_second)
            : name(name), duration(duration), ticks_per_second(ticks_per_second) {}

        void AddTrack(std::shared_ptr<AnimationTrack> track);
        std::shared_ptr<AnimationTrack> GetTrack(const std::string& bone_name);

        const std::string& GetName() const { return name; }
        float GetDuration() const { return duration; }
        float GetTicksPerSecond() const { return ticks_per_second; }
        const std::vector<std::shared_ptr<AnimationTrack>>& GetTracks() const { return tracks; }

    private:
        std::string name;
        float duration;
        float ticks_per_second;
        std::vector<std::shared_ptr<AnimationTrack>> tracks;
    };
}
