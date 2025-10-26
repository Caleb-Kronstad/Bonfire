#include "bonfire_pch.hpp"
#include "AnimationTrack.hpp"

namespace Bonfire
{
    void AnimationTrack::AddPositionKeyframe(float time, const glm::vec3& position)
    {
        position_keyframes.push_back({time, position});
    }
    void AnimationTrack::AddRotationKeyframe(float time, const glm::quat& rotation)
    {
        rotation_keyframes.push_back({time, rotation});
    }
    void AnimationTrack::AddScaleKeyframe(float time, const glm::vec3& scale)
    {
        scale_keyframes.push_back({time, scale});
    }

    template <typename T>
    int AnimationTrack::FindKeyframeIndex(const std::vector<T>& keyframes, float time) const
    {
        for (int i = 0; i < keyframes.size() - 1; i++)
        {
            if (time < keyframes[i+1].timestamp)
                return i;
        }
        return keyframes.size() - 2;
    }

    glm::vec3 AnimationTrack::InterpolatePosition(float time) const
    {
        if (position_keyframes.size() == 1)
            return position_keyframes[0].position;

        int index = FindKeyframeIndex(position_keyframes, time);
        int next_index = index+1;
        float delta_time = position_keyframes[next_index].timestamp - position_keyframes[index].timestamp;
        float factor = (time - position_keyframes[index].timestamp) / delta_time;

        return glm::mix(position_keyframes[index].position, position_keyframes[next_index].position, factor);
        
    }
    glm::quat AnimationTrack::InterpolateRotation(float time) const
    {
        if (rotation_keyframes.size() == 1)
            return rotation_keyframes[0].rotation;

        int index = FindKeyframeIndex(rotation_keyframes, time);
        int next_index = index+1;
        float delta_time = rotation_keyframes[next_index].timestamp - rotation_keyframes[index].timestamp;
        float factor = (time - rotation_keyframes[index].timestamp) / delta_time;
        
        return glm::slerp(rotation_keyframes[index].rotation, rotation_keyframes[next_index].rotation, factor);
    }
    glm::vec3 AnimationTrack::InterpolateScale(float time) const
    {
        if (scale_keyframes.size() == 1)
            return scale_keyframes[0].scale;

        int index = FindKeyframeIndex(scale_keyframes, time);
        int next_index = index+1;
        float delta_time = scale_keyframes[next_index].timestamp - scale_keyframes[index].timestamp;
        float factor = (time - scale_keyframes[index].timestamp) / delta_time;

        return glm::mix(scale_keyframes[index].scale, scale_keyframes[next_index].scale, factor);
    }

}