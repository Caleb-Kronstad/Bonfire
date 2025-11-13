#pragma once
#include "Renderer/Model.hpp"

namespace Bonfire
{
    struct PhysicsShapeData
    {
        PhysicsShapeType type;
        glm::vec3 dimensions;
    };
        
    class PhysicsBody
    {
    public:
        PhysicsBody(JPH::BodyID body_id, PhysicsBodyType body_type, PhysicsShapeData shape_data);
        ~PhysicsBody();

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale, std::shared_ptr<Model> model = nullptr);
        void SetLinearVelocity(const glm::vec3& velocity);
        void SetAngularVelocity(const glm::vec3& angular_velocity);
        void SetAllowedDOFS(bool translation_x, bool translation_y, bool translation_z, bool rotation_x, bool rotation_y, bool rotation_z);
        void SetCollisionGroup(const JPH::CollisionGroup& group);

        void AddForce(const glm::vec3& force);
        void AddImpulse(const glm::vec3& impulse);
        void AddTorque(const glm::vec3& torque);

        void SetMass(float mass);
        void SetFriction(float friction);
        void SetRestitution(float restitution);
        void SetGravityFactor(float factor);

        void SetEnabled(bool enabled);
        bool IsEnabled() const;

        JPH::BodyID GetBodyID() const { return body_id; }
        PhysicsBodyType& GetBodyType() { return body_type; }
        PhysicsShapeData& GetShapeData() { return shape_data; }

        glm::vec3 GetPosition() const;
        glm::quat GetRotation() const;
        glm::vec3 GetLinearVelocity() const;
        glm::vec3 GetAngularVelocity() const;
        float GetMass() const;

    public:
        uint32_t id = 0;
        uint32_t mesh_id = 0;
        bool enabled = true;
        std::string name = "Physics Object";

    private:
        JPH::BodyID body_id;
        PhysicsBodyType body_type;
        PhysicsShapeData shape_data;
    };
}
