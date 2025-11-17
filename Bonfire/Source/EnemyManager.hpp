#pragma once
#include "BonfireEngine.hpp"
#include "Player.hpp"

using namespace Bonfire;

class Player;

struct EnemyInstance
{
    std::shared_ptr<Entity> entity;
    std::shared_ptr<Entity> weapon;

    float health;
    float max_health = 25.0f;
    float damage = 1.0f;

    glm::vec3 spawn_position;
    bool dead = false;
    bool chasing = false;
    bool is_attacking = false;
    bool is_rotating_to_attack = false;
    bool hitbox_was_active = false;
    bool rock_collision_enabled = false;

    float attack_timer = 0.0f;
    float attack_duration = 0.0f;
    float attack_rotation_timer = 0.0f;

    std::vector<HitboxCooldown> recent_hitbox_cooldowns;
};

class EnemyManager : public Layer
{
public:
    EnemyManager();
    ~EnemyManager() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(const float& delta_time) override;
    void OnInterfaceUpdate() override;
    void OnInput(Input& input) override;

    void TakeDamage(int enemy_index, float damage, std::shared_ptr<Entity> hitbox);
    int GetEnemyIndexFromEntity(std::shared_ptr<Entity> entity);

    std::vector<EnemyInstance>& GetEnemies() { return enemies; }

private:
    void InitializeEnemies();
    void UpdateEnemy(EnemyInstance& enemy, const float& delta_time);
    void HandleEnemyState(EnemyInstance& enemy, const float& delta_time);
    void SimpleFollowPlayer(EnemyInstance& enemy, const float& delta_time);
    void AttackPlayer(EnemyInstance& enemy);
    void AttachWeaponToHand(EnemyInstance& enemy, const std::string& bone_name);

    void PlayAnimation(EnemyInstance& enemy, const std::string& animation_name);
    void StopAnimation(EnemyInstance& enemy);
    bool IsAnimationPlaying(EnemyInstance& enemy, const std::string& animation_name);

private:
    std::vector<EnemyInstance> enemies;
    std::vector<std::shared_ptr<Entity>> enemy_entities;

    Player* player_layer = nullptr;
    std::shared_ptr<Entity> player;

    float move_speed = 3.0f;
    float rotation_speed = 3.0f;
    float max_distance_from_spawn = 100.0f;
    float chase_range = 50.0f;
    float attack_range = 20.0f;
    float rotation_before_attack_threshold = 0.95f;
    float max_attack_rotation_time = 2.0f;

    glm::vec3 weapon_offset = glm::vec3(-0.5f, 3.5f, -2.0f);
    glm::vec3 forward_alignment = glm::vec3(0.0f, 0.0f, -1.0f);
    float min_alignment_to_move = -1.0f;

    const float HITBOX_ACTIVE_START = 0.17f;
    const float HITBOX_ACTIVE_END = 0.34f;
    const float DAMAGE_COOLDOWN_TIME = 1.0f;
};