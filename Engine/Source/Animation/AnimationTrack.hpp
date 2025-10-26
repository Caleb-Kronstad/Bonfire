#pragma once

namespace Bonfire
{
    struct PositionKeyFrame
    {
        float timestamp;
        glm::vec3 position;
    };
    struct RotationKeyFrame
    {
        float timestamp;
        glm::quat rotation;
    };
    struct ScaleKeyFrame
    {
        float timestamp;
        glm::vec3 scale;
    };
    
    
    class AnimationTrack
    {
    public:
        AnimationTrack(const std::string& bone_name) : bone_name(bone_name) {}

        void AddPositionKeyframe(float time, const glm::vec3& position);
        void AddRotationKeyframe(float time, const glm::quat& rotation);
        void AddScaleKeyframe(float time, const glm::vec3& scale);

        glm::vec3 InterpolatePosition(float time) const;
        glm::quat InterpolateRotation(float time) const;
        glm::vec3 InterpolateScale(float time) const;

        const std::string& GetBoneName() const { return bone_name; }

    private:
        template<typename T>
        int FindKeyframeIndex(const std::vector<T>& keyframes, float time) const;
        
    private:
        std::string bone_name;
        std::vector<PositionKeyFrame> position_keyframes;
        std::vector<RotationKeyFrame> rotation_keyframes;
        std::vector<ScaleKeyFrame> scale_keyframes;
    };
}
