#include "bonfire_pch.hpp"
#include "PhysicsObject.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
    PhysicsObject::PhysicsObject(JPH::BodyID body_id, PhysicsBodyType body_type, PhysicsShapeData shape_data)
        : body_id(body_id), body_type(body_type), shape_data(shape_data)
    {
    }

    void PhysicsObject::SetPosition(const glm::vec3& position)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetPosition(body_id, JPH::Vec3(position.x, position.y, position.z), JPH::EActivation::DontActivate);
    }

    void PhysicsObject::SetRotation(const glm::quat& rotation)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetRotation(body_id, JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::DontActivate);
    }

    glm::vec3 PhysicsObject::GetPosition() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 pos = body_interface.GetPosition(body_id);
        return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
    }

    glm::quat PhysicsObject::GetRotation() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Quat rot = body_interface.GetRotation(body_id);
        return glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
    }

    void PhysicsObject::SetLinearVelocity(const glm::vec3& velocity)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetLinearVelocity(body_id, JPH::Vec3(velocity.x, velocity.y, velocity.z));
    }

    glm::vec3 PhysicsObject::GetLinearVelocity() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 vel = body_interface.GetLinearVelocity(body_id);
        return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsObject::SetAngularVelocity(const glm::vec3& angular_velocity)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetAngularVelocity(body_id, JPH::Vec3(angular_velocity.x, angular_velocity.y, angular_velocity.z));
    }

    glm::vec3 PhysicsObject::GetAngularVelocity() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 vel = body_interface.GetAngularVelocity(body_id);
        return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsObject::AddForce(const glm::vec3& force)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddForce(body_id, JPH::Vec3(force.x, force.y, force.z));
    }

    void PhysicsObject::AddImpulse(const glm::vec3& impulse)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddImpulse(body_id, JPH::Vec3(impulse.x, impulse.y, impulse.z));
    }

    void PhysicsObject::AddTorque(const glm::vec3& torque)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddTorque(body_id, JPH::Vec3(torque.x, torque.y, torque.z));
    }

    void PhysicsObject::SetMass(float mass)
    {
        if (body_type != PhysicsBodyType::DYNAMIC || mass <= 0.0f)
            return;

        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyLockWrite lock(physics_system.GetBodyLockInterface(), body_id);
        if (lock.Succeeded())
        {
            JPH::Body& body = lock.GetBody();
            JPH::MassProperties mass_properties = body.GetShape()->GetMassProperties();
            mass_properties.ScaleToMass(mass);
            body.GetMotionProperties()->SetMassProperties(JPH::EAllowedDOFs::All, mass_properties);
        }
    }

    void PhysicsObject::SetFriction(float friction)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetFriction(body_id, friction);
    }

    void PhysicsObject::SetRestitution(float restitution)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetRestitution(body_id, restitution);
    }

    void PhysicsObject::SetGravityFactor(float factor)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetGravityFactor(body_id, factor);
    }

    void PhysicsObject::SetEnabled(bool enabled)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        if (enabled)
            body_interface.ActivateBody(body_id);
        else
            body_interface.DeactivateBody(body_id);
    }

    bool PhysicsObject::IsEnabled() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        return body_interface.IsActive(body_id);
    }
}
