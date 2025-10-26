#pragma once

#include "Animation/Skeleton.hpp"
#include "Animation/Animation.hpp"

namespace Bonfire
{
    inline constexpr unsigned int MAX_BONES = 128;
    
    class Animator
    {
    public:
        Animator(std::shared_ptr<Skeleton> skeleton);

        void AddAnimation(std::shared_ptr<Animation> animation);
        void Play(const std::string& animation_name);
        void Pause();
        void Stop();
        float& GetSpeed()  { return speed; }
        bool& GetLoop() { return loop; }

        void Update(float delta_time);

        const std::vector<glm::mat4>& GetBoneTransforms() const { return final_bone_transforms; }
        AnimationState GetState() const { return state; }
        float GetCurrentAnimationTime() const { return current_time; }
        const std::string& GetCurrentAnimationName() const { return current_animation_name; }
        std::shared_ptr<Animation> GetCurrentAnimation() const { return current_animation; }
        const std::unordered_map<std::string, std::shared_ptr<Animation>>& GetAnimations() const { return animations; }

    private:
        void CalculateBoneTransforms(std::shared_ptr<Animation> animation, float time);
        void CalculateBoneTransform(std::shared_ptr<Animation> animation, int bone_index, const glm::mat4& parent_transform);

    private:
        std::shared_ptr<Skeleton> skeleton;
        std::unordered_map<std::string, std::shared_ptr<Animation>> animations;
        std::string current_animation_name;
        std::shared_ptr<Animation> current_animation = nullptr;
        float current_time = 0.0f;
        float speed = 1.0f;
        bool loop = true;
        AnimationState state = AnimationState::STOPPED;

        std::vector<glm::mat4> final_bone_transforms;
        std::vector<glm::mat4> bone_transforms;
    };
}
