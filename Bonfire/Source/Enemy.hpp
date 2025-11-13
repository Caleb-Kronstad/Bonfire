#pragma once

#include "BonfireEngine.hpp"
#include "Player.hpp"

using namespace Bonfire;

class StoneGolem : public Layer
{
public:
    StoneGolem();
    ~StoneGolem() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const float& delta_time) override;
    void OnInterfaceUpdate() override;
    void OnInput(Input& input) override;

private:
    void AttackPlayer();
    void AttachRockToHand(const std::string& bone_name);
    void PlayAnimation(const std::string& animation_name);
    void StopAnimation();
    bool IsAnimationPlaying(const std::string& animation_name);
    void HandleState(const float& delta_time);
    void SimpleFollowPlayer(const float& delta_time);
    
    
    // UNUSED FOR NOW
    void UpdatePathfinding(const float& delta_time);
    void FollowPath(const float& delta_time);
    bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to);
    std::vector<glm::vec3> CalculatePath(const glm::vec3& start, const glm::vec3& goal);

private:
    bool dead = false;
    
    Player* player_layer = nullptr;
    std::shared_ptr<Entity> enemy;
    std::shared_ptr<Entity> player;
    std::vector<glm::vec3> current_path;

    std::shared_ptr<Entity> rock;
    glm::vec3 rock_position_offset = glm::vec3(-0.25f, -1.0f, 0.0f);
    const float HITBOX_ACTIVE_START = 0.17f;
    const float HITBOX_ACTIVE_END = 0.34f;
    bool hitbox_was_active = false;
    bool rock_collision_enabled = false;

    glm::vec3 forward_alignment = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 spawn;
    float move_speed = 3.0f;
    float rotation_speed = 3.0f;
    float max_distance_from_spawn = 25.0f;
    float chase_range = 25.0f;
    bool chasing = false;

    bool is_attacking = false;
    float attack_timer = 0.0f;
    float attack_duration;
    float attack_range = 5.0f;

    float min_alignment_to_move = -1.0f;
    float rotation_before_attack_threshold = 0.95;
    bool is_rotating_to_attack = false;
    float attack_rotation_timer = 0.0f;
    float max_attack_rotation_time = 2.0f;

    float damage = 1.0f;
    float max_health = 10.0f;
    float current_health;
};