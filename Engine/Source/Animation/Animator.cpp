#include "bonfire_pch.hpp"
#include "Animator.hpp"

#include "Core/Utility.hpp"
#include "gtx/common.hpp"

namespace Bonfire
{
    Animator::Animator(std::shared_ptr<Skeleton> skeleton)
        : skeleton(skeleton)
    {
        final_bone_transforms.resize(MAX_BONES, glm::mat4(1.0f));
        bone_transforms.resize(skeleton->GetBoneCount(), glm::mat4(1.0f));
    }
    
    void Animator::Update(float delta_time)
    {
        if (state != AnimationState::PLAYING || !current_animation)
            return;

        float ticks_per_second;
        if (!FloatEquals(current_animation->GetTicksPerSecond(), 0.0f))
            ticks_per_second = current_animation->GetTicksPerSecond();
        else
            ticks_per_second = 25.0f;

        current_time += ticks_per_second * delta_time * speed;

        if (current_time >= current_animation->GetDuration())
        {
            if (loop)
            {
                current_time = glm::fmod(current_time, current_animation->GetDuration());
            }
            else
            {
                current_time = current_animation->GetDuration();
                state = AnimationState::STOPPED;
            }
        }

        CalculateBoneTransforms(current_animation, current_time);
    }

    void Animator::AddAnimation(std::shared_ptr<Animation> animation)
    {
        animations.insert_or_assign(animation->GetName(), animation);
    }
    void Animator::Play(const std::string& animation_name)
    {
        Log::Info("Trying to play animation: '" + animation_name + "'");
        auto it = animations.find(animation_name);
        if (it != animations.end())
        {
            current_animation_name = animation_name;
            current_animation = it->second;
            state = AnimationState::PLAYING;
        }
    }
    void Animator::Pause()
    {
        if (state == AnimationState::PLAYING)
            state = AnimationState::PAUSED;
    }
    void Animator::Stop()
    {
        state = AnimationState::STOPPED;
        current_time = 0.0f;
        current_animation = nullptr;
    }
    
    void Animator::CalculateBoneTransforms(std::shared_ptr<Animation> animation, float time)
    {
        for (int i = 0; i < skeleton->GetBoneCount(); i++)
        {
            glm::mat4 parent_transform = glm::mat4(1.0f);
            int parent_index = skeleton->GetParentIndex(i);
            if (parent_index >= 0)
                parent_transform = bone_transforms[parent_index];
            CalculateBoneTransform(animation, i, parent_transform);
        }
    }
    void Animator::CalculateBoneTransform(std::shared_ptr<Animation> animation, int bone_index, const glm::mat4& parent_transform)
    {
        const Bone& bone = skeleton->GetBone(bone_index);
        std::shared_ptr<AnimationTrack> track = animation->GetTrack(bone.name);

        glm::mat4 local_transform = glm::mat4(1.0f);

        if (track)
        {
            glm::vec3 position = track->InterpolatePosition(current_time);
            glm::quat rotation = track->InterpolateRotation(current_time);
            glm::vec3 scale = track->InterpolateScale(current_time);

            glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 rotation_matrix = glm::toMat4(rotation);
            glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);

            local_transform = translation_matrix * rotation_matrix * scale_matrix;
        }

        bone_transforms[bone_index] = parent_transform * local_transform;
        final_bone_transforms[bone_index] = bone_transforms[bone_index] * bone.offset_matrix;
    }
}
