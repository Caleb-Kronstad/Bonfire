#include "Enemy.hpp"

Enemy::Enemy() : Layer("New Layer")
{

}

Enemy::~Enemy()
{

}

void Enemy::OnAttach()
{
    Project& project = Project::GetInstance();
    Renderer& renderer = Project::GetRenderer();
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    AudioSystem& audio_system = Project::GetAudioSystem();
    ScriptSystem& script_system = Project::GetScriptSystem();
    Scene& scene = renderer.GetScene();

    enemy = scene.GetEntityByName("TestDummy");
    player = scene.GetEntityByName("Player");

    spawn = enemy->position;
}

void Enemy::OnDetach()
{

}

void Enemy::OnUpdate(const float& delta_time)
{
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    
    if (!enemy || !player) return;

    if (!enemy->HasComponent<PhysicsComponent>() || !player->HasComponent<PhysicsComponent>()) return;

    HandleState(delta_time);
}

void Enemy::OnInterfaceUpdate()
{

}

void Enemy::OnInput(Input& input)
{
}

void Enemy::HandleState(const float& delta_time)
{
    PhysicsComponent& enemy_physics = enemy->GetComponent<PhysicsComponent>();
    PhysicsComponent& player_physics = player->GetComponent<PhysicsComponent>();
    PhysicsBody& enemy_body = *enemy_physics.physics_body;
    PhysicsBody& player_body = *player_physics.physics_body;
    
    float distance_from_spawn = glm::distance(enemy->position, spawn);
    if (distance_from_spawn > max_distance_from_spawn)
        chasing = false;

    float distance_from_player = glm::distance(enemy->position, player->position);
    if (distance_from_player < chase_range)
        chasing = true;

    if (is_attacking)
    {
        attack_timer += delta_time;

        glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
        enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
        enemy_body.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

        if (attack_timer >= attack_duration)
        {
            is_attacking = false;
            attack_timer = 0.0f;
        }
    }
    else if (chasing)
    {
        if (distance_from_player < attack_range)
        {
            glm::quat current_rotation = enemy_body.GetRotation();
            glm::vec3 forward = current_rotation * forward_alignment;
            glm::vec3 forward_2d = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

            glm::vec3 to_player = player->position - enemy->position;
            glm::vec3 to_player_2d = glm::normalize(glm::vec3(to_player.x, 0.0f, to_player.z));

            float alignment = glm::dot(forward_2d, to_player_2d);

            if (alignment < rotation_before_attack_threshold)
            {
                    is_rotating_to_attack = true;
                    attack_rotation_timer += delta_time;

                    float target_yaw = atan2f(-to_player_2d.z, to_player_2d.x);
                    glm::quat target_rotation = glm::angleAxis(target_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::quat new_rotation = glm::slerp(current_rotation, target_rotation, rotation_speed * delta_time);
                    enemy_body.SetRotation(new_rotation);

                    glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
                    enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
                    enemy_body.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

                    if (attack_rotation_timer > max_attack_rotation_time)
                    {
                            is_rotating_to_attack = false;
                            attack_rotation_timer = 0.0f;
                            current_path.clear();
                    }
            }
            else
            {
                    is_rotating_to_attack = false;
                    attack_rotation_timer = 0.0f;
                    is_attacking = true;
                    attack_timer = 0.0f;
                    AttackPlayer();

                    glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
                    enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
                    enemy_body.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    current_path.clear();
            }
        }
        else
        {
            if (use_simple_movement)
            {
                SimpleFollowPlayer(delta_time);
            }
            else
            {
                UpdatePathfinding(delta_time);
                FollowPath(delta_time);
            }
        }
    }
    else
    {
        glm::vec3 current_velocity = enemy_body.GetLinearVelocity();
        enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
        current_path.clear();
    }
}

void Enemy::AttackPlayer()
{
    Log::Info("ATTACKING PLAYER");
}

void Enemy::SimpleFollowPlayer(const float& delta_time)
{
    if (!enemy->HasComponent<PhysicsComponent>())
        return;

    auto& enemy_physics = enemy->GetComponent<PhysicsComponent>();
    PhysicsBody& enemy_body = *enemy_physics.physics_body;

    glm::vec3 to_player = player->position - enemy->position;
    glm::vec3 to_player_2d = glm::normalize(glm::vec3(to_player.x, 0.0f, to_player.z));

    glm::quat current_rotation = enemy_body.GetRotation();
    glm::vec3 forward = current_rotation * forward_alignment;
    glm::vec3 forward_2d = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    float alignment = glm::dot(forward_2d, to_player_2d);

    float target_yaw = atan2f(-to_player_2d.z, to_player_2d.x);
    glm::quat target_rotation = glm::angleAxis(target_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat new_rotation = glm::slerp(current_rotation, target_rotation, rotation_speed * delta_time);
    enemy_body.SetRotation(new_rotation);

    glm::vec3 current_velocity = enemy_body.GetLinearVelocity();

    if (alignment > min_alignment_to_move)
    {
        float speed_scale = (glm::max)(alignment, 0.0f);
        glm::vec3 desired_velocity = forward_2d * (move_speed * speed_scale);
        desired_velocity.y = current_velocity.y;
        enemy_body.SetLinearVelocity(desired_velocity);
    }
    else
    {
        enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
    }
}

void Enemy::UpdatePathfinding(const float& delta_time)
{
    bool need_recalculation = false;

    if (current_path.empty()) need_recalculation = true;
    else if (!current_path.empty())
    {
        glm::vec3 next_waypoint = current_path.front();
        if (!HasLineOfSight(enemy->position, next_waypoint))
            need_recalculation = true;
    }

    if (need_recalculation)
        current_path = CalculatePath(enemy->position, player->position);
}

void Enemy::FollowPath(const float& delta_time)
{
    if (current_path.empty() || !enemy->HasComponent<PhysicsComponent>())
                return;

    auto& enemy_physics = enemy->GetComponent<PhysicsComponent>();
    PhysicsBody& enemy_body = *enemy_physics.physics_body;

    glm::vec3 current_pos = enemy->position;
    glm::vec3 target_pos = current_path.front();

    float distance_to_waypoint = glm::distance(current_pos, target_pos);
    if (distance_to_waypoint < 1.0f)
    {
            current_path.erase(current_path.begin());

            if (!current_path.empty())
                    target_pos = current_path.front();
            else
                    return;
    }

    glm::vec3 direction_to_target = glm::normalize(target_pos - current_pos);
    glm::vec3 target_2d = glm::normalize(glm::vec3(direction_to_target.x, 0.0f, direction_to_target.z));

    glm::quat current_rotation = enemy_body.GetRotation();
    glm::vec3 forward = current_rotation * forward_alignment;
    glm::vec3 forward_2d = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));

    float alignment = glm::dot(forward_2d, target_2d);
    float target_yaw = atan2f(-target_2d.z, target_2d.x);
    glm::quat target_rotation = glm::angleAxis(target_yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::quat new_rotation = glm::slerp(current_rotation, target_rotation, rotation_speed * delta_time);
    enemy_body.SetRotation(new_rotation);

    glm::vec3 current_velocity = enemy_body.GetLinearVelocity();

    if (alignment > min_alignment_to_move)
    {
            float speed_scale = (glm::max)(alignment, 0.0f); 
            glm::vec3 desired_velocity = forward_2d * (move_speed * speed_scale);
            desired_velocity.y = current_velocity.y;
            enemy_body.SetLinearVelocity(desired_velocity);
    }
    else
    {
            enemy_body.SetLinearVelocity(glm::vec3(0.0f, current_velocity.y, 0.0f));
    }
}

bool Enemy::HasLineOfSight(const glm::vec3& from, const glm::vec3& to)
{
    PhysicsSystem& physics_system = Project::GetPhysicsSystem();
    const JPH::NarrowPhaseQuery& narrow_phase = physics_system.GetNarrowPhaseQuery();

    glm::vec3 direction = to - from;
    float distance = glm::length(direction);
    direction = glm::normalize(direction);

    JPH::RVec3 ray_origin(from.x, from.y, from.z);
    JPH::Vec3 ray_direction(direction.x, direction.y, direction.z);

    JPH::RRayCast ray(ray_origin, ray_direction * distance);
    JPH::RayCastResult hit;
    bool had_hit = narrow_phase.CastRay(ray, hit);

    return !had_hit;
}

std::vector<glm::vec3> Enemy::CalculatePath(const glm::vec3& start, const glm::vec3& goal)
{
    std::vector<glm::vec3> path;
  
    if (HasLineOfSight(start, goal))
    {
        path.push_back(goal);
        return path;
    }

    glm::vec3 current = start;
    glm::vec3 target = goal;
    int max_iterations = 10;
    int iteration = 0;
    while (!HasLineOfSight(current, target) && iteration < max_iterations)
    {
        iteration++;
        glm::vec3 to_target = glm::normalize(target - current);
        const float angles[] = { 45.0f, -45.0f, 90.0f, -90.0f, 135.0f, -135.0f };
        const float probe_distance = 5.0f;
        bool found_waypoint = false;
        glm::vec3 best_waypoint = current;
        for (float angle_deg : angles)
        {
            float angle_rad = glm::radians(angle_deg);
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle_rad, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::vec3 probe_dir = glm::vec3(rotation * glm::vec4(to_target, 0.0f));
            glm::vec3 probe_point = current + probe_dir * probe_distance;
            if (HasLineOfSight(current, probe_point))
            {
                best_waypoint = probe_point;
                found_waypoint = true;
                if (HasLineOfSight(probe_point, target))
                {
                    path.push_back(best_waypoint);
                    path.push_back(target);
                    return path;
                }
                break;
            }
        }
        if (found_waypoint)
        {
            path.push_back(best_waypoint);
            current = best_waypoint;
        }
        else
        {
            path.push_back(target);
            break;
        }
    }
    if (path.empty() || glm::distance(path.back(), target) > 0.1f)
    {
        path.push_back(target);
    }
    return path;
}