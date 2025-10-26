#include "bonfire_pch.hpp"
#include "Animation.hpp"

namespace Bonfire
{
    void Animation::AddTrack(std::shared_ptr<AnimationTrack> track)
    {
        tracks.push_back(track);
    }
    std::shared_ptr<AnimationTrack> Animation::GetTrack(const std::string& bone_name)
    {
        for (std::shared_ptr<AnimationTrack> track : tracks)
        {
            if (track->GetBoneName() == bone_name)
                return track;
        }
        return nullptr;
    }
}
