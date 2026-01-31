#pragma once

#include "Command.hpp"
#include "Renderer/Entity.hpp"
#include "Renderer/Scene.hpp"
#include "Physics/PhysicsManager.hpp"

namespace Bonfire
{
    class SetTransformCommand : public Command
    {
    public:
        SetTransformCommand(Scene* scene, PhysicsManager* physics_system, uint32_t entity_id,
            const glm::vec3& old_position, const glm::vec3& old_rotation, const glm::vec3& old_scale,
              const glm::vec3& new_position, const glm::vec3& new_rotation, const glm::vec3& new_scale)
        : scene(scene), physics_system(physics_system), entity_id(entity_id),
        old_position(old_position), old_rotation(old_rotation), old_scale(old_scale),
        new_position(new_position), new_rotation(new_rotation), new_scale(new_scale)
        {
        }
        
        void Execute() override
        {
            ApplyTransform(new_position, new_rotation, new_scale);
        }

        void Undo() override
        {
            ApplyTransform(old_position, old_rotation, old_scale);
        }

        std::string GetDescription() const override
        {
            return "Transform";
        }

    private:
        Scene* scene;
        PhysicsManager* physics_system;
        uint32_t entity_id;
        glm::vec3 old_position, old_rotation, old_scale;
        glm::vec3 new_position, new_rotation, new_scale;

        void ApplyTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
        {
            std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities = scene->GetEntities();
            if (entities.contains(entity_id))
            {
                auto& entity = entities.at(entity_id);
                entity->position = position;
                entity->rotation = rotation;
                entity->scale = scale;
                entity->UpdateComponents();
            }
        }
    };
}