#include "bonfire_pch.hpp"
#include "PhysicsBody.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
    PhysicsBody::PhysicsBody(JPH::BodyID body_id, PhysicsBodyType body_type, PhysicsShapeData shape_data)
        : body_id(body_id), body_type(body_type), shape_data(shape_data)
    {
    }

    PhysicsBody::~PhysicsBody()
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();

        if (body_interface.IsAdded(body_id))
        {
            body_interface.RemoveBody(body_id);
            body_interface.DestroyBody(body_id);
        }
    }

    void PhysicsBody::SetAllowedDOFS(bool translation_x, bool translation_y, bool translation_z, bool rotation_x, bool rotation_y, bool rotation_z)
    {
        if (body_type != PhysicsBodyType::DYNAMIC) return;

        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyLockWrite lock(physics_system.GetBodyLockInterface(), body_id);
        if (lock.Succeeded())
        {
            JPH::Body& body = lock.GetBody();

            JPH::EAllowedDOFs allowed_dofs = JPH::EAllowedDOFs::All;
            if (translation_x && translation_y && translation_z && rotation_x && rotation_y && rotation_z)
                allowed_dofs = JPH::EAllowedDOFs::All;
            else if (translation_x && translation_y && translation_z && !rotation_x && !rotation_y && !rotation_z)
                allowed_dofs = JPH::EAllowedDOFs::TranslationX | JPH::EAllowedDOFs::TranslationY | JPH::EAllowedDOFs::TranslationZ;
            else if (translation_x && translation_y && translation_z && !rotation_x && rotation_y && !rotation_z)
                allowed_dofs = JPH::EAllowedDOFs::Plane2D;

            JPH::MassProperties mass_properties = body.GetShape()->GetMassProperties();
            body.GetMotionProperties()->SetMassProperties(allowed_dofs, mass_properties);
        }
    }
    
    void PhysicsBody::SetPosition(const glm::vec3& position)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetPosition(body_id, JPH::Vec3(position.x, position.y, position.z), JPH::EActivation::DontActivate);
    }

    void PhysicsBody::SetRotation(const glm::quat& rotation)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetRotation(body_id, JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::DontActivate);
    }

    void PhysicsBody::SetScale(const glm::vec3& scale)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();

        JPH::Ref<JPH::Shape> new_shape;

        switch (shape_data.type)
        {
        case PhysicsShapeType::BOX:
            {
                glm::vec3 scaled_half_extents = scale;
                shape_data.dimensions = scaled_half_extents;
    
                if (scaled_half_extents.x <= 0 || scaled_half_extents.y <= 0 || scaled_half_extents.z <= 0)
                {
                    Log::Error("Box dimensions must be positive");
                    return;
                }
    
                JPH::BoxShapeSettings shape_settings(JPH::Vec3(scaled_half_extents.x, scaled_half_extents.y, scaled_half_extents.z), 0.01f);
                auto shape_result = shape_settings.Create();
    
                new_shape = shape_result.Get();
                break;
            }
        case PhysicsShapeType::SPHERE:
            {
                float scaled_radius = shape_data.dimensions.x * (glm::max)((glm::max)(scale.x, scale.y), scale.z);
                JPH::SphereShapeSettings shape_settings(scaled_radius);
                new_shape = shape_settings.Create().Get();
                break;
            }
        case PhysicsShapeType::CAPSULE:
            {
                float scaled_radius = shape_data.dimensions.x * (glm::max)(scale.x, scale.z);
                float scaled_half_height = shape_data.dimensions.y * scale.y;
                JPH::CapsuleShapeSettings shape_settings(scaled_half_height, scaled_radius);
                new_shape = shape_settings.Create().Get();
                break;
            }
        }

        body_interface.SetShape(body_id, new_shape.GetPtr(), true, JPH::EActivation::DontActivate);
    }

    glm::vec3 PhysicsBody::GetPosition() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 pos = body_interface.GetPosition(body_id);
        return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
    }

    glm::quat PhysicsBody::GetRotation() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Quat rot = body_interface.GetRotation(body_id);
        return glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
    }

    void PhysicsBody::SetLinearVelocity(const glm::vec3& velocity)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetLinearVelocity(body_id, JPH::Vec3(velocity.x, velocity.y, velocity.z));
    }

    glm::vec3 PhysicsBody::GetLinearVelocity() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 vel = body_interface.GetLinearVelocity(body_id);
        return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsBody::SetAngularVelocity(const glm::vec3& angular_velocity)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetAngularVelocity(body_id, JPH::Vec3(angular_velocity.x, angular_velocity.y, angular_velocity.z));
    }

    glm::vec3 PhysicsBody::GetAngularVelocity() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 vel = body_interface.GetAngularVelocity(body_id);
        return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsBody::AddForce(const glm::vec3& force)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddForce(body_id, JPH::Vec3(force.x, force.y, force.z));
    }

    void PhysicsBody::AddImpulse(const glm::vec3& impulse)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddImpulse(body_id, JPH::Vec3(impulse.x, impulse.y, impulse.z));
    }

    void PhysicsBody::AddTorque(const glm::vec3& torque)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddTorque(body_id, JPH::Vec3(torque.x, torque.y, torque.z));
    }

    void PhysicsBody::SetMass(float mass)
    {
        if (body_type != PhysicsBodyType::DYNAMIC || mass <= 0.0f) return;

        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyLockWrite lock(physics_system.GetBodyLockInterface(), body_id);
        if (lock.Succeeded())
        {
            JPH::Body& body = lock.GetBody();
            JPH::MassProperties mass_properties = body.GetShape()->GetMassProperties();
            mass_properties.ScaleToMass(mass);
            JPH::EAllowedDOFs current_dofs = body.GetMotionProperties()->GetAllowedDOFs();
            body.GetMotionProperties()->SetMassProperties(current_dofs, mass_properties);
        }
    }

    float PhysicsBody::GetMass() const
    {
        if (body_type != PhysicsBodyType::DYNAMIC) return 0.0f;

        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyLockRead lock(physics_system.GetBodyLockInterface(), body_id);
        if (lock.Succeeded())
        {
            const JPH::Body& body = lock.GetBody();
            return 1.0f / body.GetMotionProperties()->GetInverseMass();
        }
        return 0.0f;
    }


    void PhysicsBody::SetFriction(float friction)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetFriction(body_id, friction);
    }

    void PhysicsBody::SetRestitution(float restitution)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetRestitution(body_id, restitution);
    }

    void PhysicsBody::SetGravityFactor(float factor)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetGravityFactor(body_id, factor);
    }

    void PhysicsBody::SetEnabled(bool enabled)
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        if (enabled)
            body_interface.ActivateBody(body_id);
        else
            body_interface.DeactivateBody(body_id);
    }

    bool PhysicsBody::IsEnabled() const
    {
        auto& physics_system = Project::GetPhysicsSystem();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        return body_interface.IsActive(body_id);
    }
}
