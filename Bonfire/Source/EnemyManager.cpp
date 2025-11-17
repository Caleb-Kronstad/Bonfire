#include "EnemyManager.hpp"
#include "Player.hpp"

EnemyManager::EnemyManager() : Layer("EnemyManager") {}
EnemyManager::~EnemyManager() {}

void EnemyManager::OnAttach()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = Project::GetRenderer();
    Scene& scene = renderer.GetScene();

    player = scene.GetEntityOfName("Player");

    for (auto& layer : project.GetLayers())
    {
        if (Player* player_ptr = dynamic_cast<Player*>(layer.get()))
        {
            player_layer = player_ptr;
            break;
        }
    }

    InitializeEnemies();
}

void EnemyManager::OnDetach()
{
    for (auto& enemy : enemies)
    {
        enemy.chasing = false;
        enemy.is_attacking = false;
        enemy.hitbox_was_active = false;
        enemy.rock_collision_enabled = false;
        enemy.is_rotating_to_attack = false;
    }
}


void EnemyManager::InitializeEnemies()
{
    Renderer& renderer = Project::GetRenderer();
    Scene& scene = renderer.GetScene();
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();

    enemy_entities = scene.GetAllEntitiesOfName("StoneGolem");

    enemies.clear();
    enemies.reserve(enemy_entities.size());

    JPH::Ref<JPH::GroupFilterTable> collision_filter = physics_system.Filter("game_entities", 4);

    for (size_t i = 0; i < enemy_entities.size(); i++)
    {
        auto& golem_entity = enemy_entities[i];

        EnemyInstance enemy;
        enemy.entity = golem_entity;
        enemy.spawn_position = golem_entity->position;
        enemy.health = enemy.max_health;

        std::string rock_name = golem_entity->name + "_Rock_" + std::to_string(i);
        enemy.weapon = scene.GetEntityOfName(rock_name);

        if (golem_entity->HasComponent<PhysicsComponent>())
        {
            PhysicsComponent& physics_component = golem_entity->GetComponent<PhysicsComponent>();
            physics_component.physics_body->SetCollisionGroup(JPH::CollisionGroup(collision_filter, 0, 1));
            physics_component.physics_body->SetMass(100.0f);
        }

        if (enemy.weapon && enemy.weapon->HasComponent<PhysicsComponent>())
        {
            PhysicsComponent& physics_component = enemy.weapon->GetComponent<PhysicsComponent>();
            physics_component.physics_body->SetCollisionGroup(JPH::CollisionGroup(collision_filter, 0, 0));
            physics_system.RegisterBodyEntity(physics_component.physics_body->GetBodyID(), enemy.weapon);
            enemy.rock_collision_enabled = false;
        }

        if (golem_entity->HasComponent<AnimationComponent>())
        {
            AnimationComponent& animation_component = golem_entity->GetComponent<AnimationComponent>();
            Animator& animator = *animation_component.animator;

            if (auto attack_anim = animator.GetAnimation("Armature|ATTACK"))
                enemy.attack_duration = attack_anim->GetDuration() / 24.0f;
            else
                Log::Error("ATTACK animation not found!");
        }

        enemies.push_back(enemy);
        PlayAnimation(enemies.back(), "Armature|IDLE");
    }

    Log::Info("EnemyManager: Initialized " + std::to_string(enemies.size()) + " enemies");
}

void EnemyManager::OnUpdate(const float& delta_time)
{
    if (!player || !player_layer) return;

    for (auto& enemy : enemies)
    {
        if (!enemy.dead)
        {
            UpdateEnemy(enemy, delta_time);
        }
    }
}

void EnemyManager::UpdateEnemy(EnemyInstance& enemy, const float& delta_time)
{
    if (!enemy.entity || !enemy.weapon) return;

    HandleEnemyState(enemy, delta_time);
    AttachWeaponToHand(enemy, "HandR");

    for (auto it = enemy.recent_hitbox_cooldowns.begin(); it != enemy.recent_hitbox_cooldowns.end();)
    {
        it->timer -= delta_time;
        if (it->timer <= 0.0f)
            it = enemy.recent_hitbox_cooldowns.erase(it);
        else
            ++it;
    }
}

void EnemyManager::HandleEnemyState(EnemyInstance& enemy, const float& delta_time)
{
    if (enemy.dead) return;

    if (!enemy.entity->HasComponent<PhysicsComponent>() || !player->HasComponent<PhysicsComponent>()) return;

    PhysicsComponent& enemy_physics = enemy.entity->GetComponent<PhysicsComponent>();
    PhysicsBody& enemy_body = *enemy_physics.physics_body;

    float distance_from_spawn = glm::distance(enemy.entity->position, enemy.spawn_position);
    if (distance_from_spawn > max_distance_from_spawn)
        enemy.chasing = false;

    float distance_from_player = glm::distance(enemy.entity->position, player->position);
    if (distance_from_player < chase_range)
        enemy.chasing = true;

    if (enemy.is_attacking)
    {
        PlayAnimation(enemy, "Armature|ATTACK");

        enemy.attack_timer += delta_time;

        float attack_progress = enemy.attack_timer / enemy.attack_duration;
        bool should_hitbox_be_active = (attack_progress >= HITBOX_ACTIVE_START && attack_progress <= HITBOX_ACTIVE_END);

        if (enemy.weapon)
        {
            if (should_hitbox_be_active && !enemy.hitbox_was_active)
            {
                enemy.rock_collision_enabled = true;

                if (enemy.weapon->HasComponent<AudioComponent>())
                    enemy.weapon->GetComponent<AudioComponent>().audio->Play();
            }
            else if (!should_hitbox_be_active && enemy.hitbox_was_active)
            {
                enemy.rock_collision_enabled = false;
            }
            enemy.hitbox_was_active = should_hitbox_be_active;
        }

        if (enemy.attack_timer >= enemy.attack_duration)
        {
            enemy.is_attacking = false;
            enemy.attack_timer = 0.0f;
            enemy.rock_collision_enabled = false;
            enemy.hitbox_was_active = false;
            StopAnimation(enemy);
            PlayAnimation(enemy, "Armature|IDLE");
        }

        glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
        enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
        enemy_body.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    else if (enemy.chasing)
    {
        if (distance_from_player < attack_range)
        {
            glm::quat current_rotation = enemy_body.GetRotation();
            glm::vec3 forward = current_rotation * forward_alignment;
            glm::vec3 forward_2d = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

            glm::vec3 to_player = player->position - enemy.entity->position;
            glm::vec3 to_player_2d = glm::normalize(glm::vec3(to_player.x, 0.0f, to_player.z));

            float alignment = -glm::dot(forward_2d, to_player_2d);

            if (alignment < rotation_before_attack_threshold)
            {
                enemy.is_rotating_to_attack = true;
                enemy.attack_rotation_timer += delta_time;

                float target_yaw = atan2f(to_player_2d.x, to_player_2d.z);
                glm::quat target_rotation = glm::angleAxis(target_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::quat new_rotation = glm::slerp(current_rotation, target_rotation, rotation_speed * delta_time);
                enemy_body.SetRotation(new_rotation);

                glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
                enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
                enemy_body.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

                if (enemy.attack_rotation_timer > max_attack_rotation_time)
                {
                    enemy.is_rotating_to_attack = false;
                    enemy.attack_rotation_timer = 0.0f;
                }
            }
            else
            {
                StopAnimation(enemy);
                enemy.is_rotating_to_attack = false;
                enemy.attack_rotation_timer = 0.0f;
                enemy.is_attacking = true;
                enemy.attack_timer = 0.0f;

                glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
                enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
                enemy_body.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
            }
        }
        else
        {
            PlayAnimation(enemy, "Armature|WALK");
            SimpleFollowPlayer(enemy, delta_time);
        }
    }
    else
    {
        PlayAnimation(enemy, "Armature|IDLE");
        glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
        enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
    }

    if (enemy.is_attacking && enemy.weapon && enemy.rock_collision_enabled)
        AttackPlayer(enemy);
}

void EnemyManager::SimpleFollowPlayer(EnemyInstance& enemy, const float& delta_time)
{
    if (enemy.dead) return;

    if (!enemy.entity->HasComponent<PhysicsComponent>()) return;

    auto& enemy_physics = enemy.entity->GetComponent<PhysicsComponent>();
    PhysicsBody& enemy_body = *enemy_physics.physics_body;

    glm::vec3 to_player = player->position - enemy.entity->position;
    glm::vec3 to_player_2d = glm::normalize(glm::vec3(to_player.x, 0.0f, to_player.z));

    glm::quat current_rotation = enemy_body.GetRotation();

    float target_yaw = atan2f(to_player_2d.x, to_player_2d.z);
    glm::quat target_rotation = glm::angleAxis(target_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat new_rotation = glm::slerp(current_rotation, target_rotation, rotation_speed * delta_time);
    enemy_body.SetRotation(new_rotation);

    glm::vec3 new_forward = new_rotation * forward_alignment;
    glm::vec3 new_forward_2d = glm::normalize(glm::vec3(new_forward.x, 0.0f, new_forward.z));

    float alignment = glm::dot(new_forward_2d, to_player_2d);
    glm::vec3 current_velocity = enemy_body.GetLinearVelocity();

    if (alignment > min_alignment_to_move)
    {
        float speed_scale = -glm::abs(alignment);
        glm::vec3 desired_velocity = new_forward_2d * (move_speed * speed_scale);
        desired_velocity.y = current_velocity.y;
        enemy_body.SetLinearVelocity(desired_velocity);
    }
    else
    {
        enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
    }
}

void EnemyManager::AttackPlayer(EnemyInstance& enemy)
{
    if (enemy.dead) return;

    if (!player || !enemy.weapon || !player_layer) return;
    if (!enemy.rock_collision_enabled) return;
    if (!enemy.weapon->HasComponent<PhysicsComponent>() || !player->HasComponent<PhysicsComponent>()) return;

    PhysicsComponent& hitbox_physics = enemy.weapon->GetComponent<PhysicsComponent>();
    PhysicsComponent& player_physics = player->GetComponent<PhysicsComponent>();
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();

    JPH::BodyID hitbox_body_id = hitbox_physics.physics_body->GetBodyID();
    JPH::BodyID player_body_id = player_physics.physics_body->GetBodyID();

    if (physics_system.AreBodiesColliding(hitbox_body_id, player_body_id))
        player_layer->TakeDamage(enemy.damage, enemy.weapon);
}

void EnemyManager::AttachWeaponToHand(EnemyInstance& enemy, const std::string& bone_name)
{
    if (enemy.dead) return;

    Renderer& renderer = Project::GetRenderer();
    Scene& scene = renderer.GetScene();

    if (!enemy.weapon || !enemy.entity->HasComponent<AnimationComponent>()) return;

    AnimationComponent& animation_component = enemy.entity->GetComponent<AnimationComponent>();
    if (!animation_component.animator || !animation_component.animator->skeleton)
    {
        Log::Error("Animator or skeleton is null");
        return;
    }

    glm::mat4 bone_transform = animation_component.animator->GetBoneWorldTransform(bone_name);
    glm::mat4 entity_world = enemy.entity->GetWorldTransformMatrix(scene.GetEntities());

    glm::mat4 model_rotation_offset = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model_rotation_offset = glm::translate(model_rotation_offset, glm::vec3(0.0f, 0.0f, -1.0f));

    glm::mat4 attachment_world = entity_world * model_rotation_offset * bone_transform;

    glm::vec3 new_position = glm::vec3(attachment_world[3]);
    glm::quat new_rotation = glm::normalize(glm::quat_cast(attachment_world));

    glm::vec3 rotated_offset = new_rotation * weapon_offset;
    new_position += rotated_offset;

    enemy.weapon->position = new_position;
    enemy.weapon->rotation = glm::eulerAngles(new_rotation);

    if (enemy.weapon->HasComponent<PhysicsComponent>())
    {
        PhysicsComponent& physics = enemy.weapon->GetComponent<PhysicsComponent>();
        physics.physics_body->SetPosition(new_position);
        physics.physics_body->SetRotation(new_rotation);
        physics.physics_body->SetLinearVelocity(glm::vec3(0.0f));
        physics.physics_body->SetAngularVelocity(glm::vec3(0.0f));
    }
}

void EnemyManager::TakeDamage(int enemy_index, float damage, std::shared_ptr<Entity> hitbox)
{
    if (enemy_index < 0 || enemy_index >= static_cast<int>(enemies.size())) return;

    auto& enemy = enemies[enemy_index];
    if (enemy.dead) return;
    if (!hitbox) return;

    for (const auto& cooldown : enemy.recent_hitbox_cooldowns)
    {
        if (cooldown.hitbox == hitbox)
            return;
    }

    enemy.health -= damage;
    enemy.recent_hitbox_cooldowns.emplace_back(hitbox, DAMAGE_COOLDOWN_TIME);

    Log::Warning("Golem hit!");
    Log::Warning(std::to_string(enemy.health));

    if (enemy.health <= 0)
    {
        enemy.health = 0;
        enemy.entity->GetComponent<ModelComponent>().enabled = false;
        enemy.dead = true;

        PhysicsSystem& physics_system = Project::GetPhysicsSystem();
        PhysicsComponent& rock_physics = enemy.weapon->GetComponent<PhysicsComponent>();
        rock_physics.physics_body = physics_system.CreateSphereBody(
            rock_physics.physics_body->GetPosition(),
            rock_physics.physics_body->GetShapeData().dimensions.x,
            PhysicsBodyType::DYNAMIC
        );
    }
}

int EnemyManager::GetEnemyIndexFromEntity(std::shared_ptr<Entity> entity)
{
    for (size_t i = 0; i < enemies.size(); i++)
    {
        if (enemies[i].entity == entity || enemies[i].weapon == entity)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void EnemyManager::PlayAnimation(EnemyInstance& enemy, const std::string& animation_name)
{
    if (!enemy.entity->HasComponent<AnimationComponent>()) return;
    AnimationComponent& animation_component = enemy.entity->GetComponent<AnimationComponent>();
    Animator& animator = *animation_component.animator;

    if (animator.GetCurrentAnimation() != nullptr && animator.GetCurrentAnimation()->GetName() != animation_name)
        animator.Stop();

    if (animator.GetState() != AnimationState::PLAYING)
        animator.Play(animation_name);
}

void EnemyManager::StopAnimation(EnemyInstance& enemy)
{
    if (!enemy.entity->HasComponent<AnimationComponent>()) return;
    AnimationComponent& animation_component = enemy.entity->GetComponent<AnimationComponent>();
    Animator& animator = *animation_component.animator;
    animator.Stop();
}

bool EnemyManager::IsAnimationPlaying(EnemyInstance& enemy, const std::string& animation_name)
{
    if (!enemy.entity->HasComponent<AnimationComponent>()) return false;
    AnimationComponent& animation_component = enemy.entity->GetComponent<AnimationComponent>();
    Animator& animator = *animation_component.animator;
    if (animator.GetCurrentAnimation() && animator.GetCurrentAnimation()->GetName() == animation_name)
        return true;
    return false;
}

void EnemyManager::OnInterfaceUpdate() {}
void EnemyManager::OnInput(Input& input) {}