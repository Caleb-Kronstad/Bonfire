#include "bonfire_pch.hpp"
#include "PhysicsBody.hpp"

#include "Core/Engine.hpp"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"

namespace Bonfire
{
    PhysicsBody::PhysicsBody(JPH::BodyID body_id, PhysicsBodyType body_type, PhysicsShapeData shape_data)
        : body_id(body_id), body_type(body_type), shape_data(shape_data)
    {
    }

    PhysicsBody::~PhysicsBody()
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();

        if (body_interface.IsAdded(body_id))
        {
            body_interface.RemoveBody(body_id);
            body_interface.DestroyBody(body_id);
        }
    }

    void PhysicsBody::SetAllowedDOFS(bool translation_x, bool translation_y, bool translation_z, bool rotation_x,
                                     bool rotation_y, bool rotation_z)
    {
        if (body_type != PhysicsBodyType::DYNAMIC) return;

        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyLockWrite lock(physics_system.GetBodyLockInterface(), body_id);
        if (lock.Succeeded())
        {
            JPH::Body& body = lock.GetBody();

            // Build allowed DOFs by OR-ing individual flags
            JPH::EAllowedDOFs allowed_dofs = (JPH::EAllowedDOFs)0;

            if (translation_x) allowed_dofs = allowed_dofs | JPH::EAllowedDOFs::TranslationX;
            if (translation_y) allowed_dofs = allowed_dofs | JPH::EAllowedDOFs::TranslationY;
            if (translation_z) allowed_dofs = allowed_dofs | JPH::EAllowedDOFs::TranslationZ;
            if (rotation_x) allowed_dofs = allowed_dofs | JPH::EAllowedDOFs::RotationX;
            if (rotation_y) allowed_dofs = allowed_dofs | JPH::EAllowedDOFs::RotationY;
            if (rotation_z) allowed_dofs = allowed_dofs | JPH::EAllowedDOFs::RotationZ;

            float current_mass = 1.0f / body.GetMotionProperties()->GetInverseMass();
            JPH::MassProperties mass_properties = body.GetShape()->GetMassProperties();
            mass_properties.ScaleToMass(current_mass);
            body.GetMotionProperties()->SetMassProperties(allowed_dofs, mass_properties);
        }
    }

    void PhysicsBody::SetCollisionGroup(const JPH::CollisionGroup& group)
    {
        PhysicsManager& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetCollisionGroup(body_id, group);
    }

    void PhysicsBody::SetPosition(const glm::vec3& position)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetPosition(body_id, JPH::Vec3(position.x, position.y, position.z),
                                   JPH::EActivation::DontActivate);
    }

    void PhysicsBody::SetRotation(const glm::quat& rotation)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetRotation(body_id, JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
                                   JPH::EActivation::DontActivate);
    }

    void PhysicsBody::SetScale(const glm::vec3& scale, std::shared_ptr<Model> model)
    {
        PhysicsManager& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Ref<JPH::Shape> new_shape = nullptr;

        switch (shape_data.type)
        {
        case PhysicsShapeType::BOX:
            {
                JPH::Ref<JPH::BoxShape> box_shape = new JPH::BoxShape(
                    JPH::Vec3(shape_data.dimensions.x * scale.x, 
                              shape_data.dimensions.y * scale.y, 
                              shape_data.dimensions.z * scale.z)
                );
                new_shape = box_shape;
                break;
            }
        case PhysicsShapeType::SPHERE:
            {
                float max_scale = (glm::max)((glm::max)(scale.x, scale.y), scale.z);
                float scaled_radius = shape_data.dimensions.x * max_scale;
                JPH::Ref<JPH::SphereShape> sphere_shape = new JPH::SphereShape(scaled_radius);
                new_shape = sphere_shape;
                break;
            }
        case PhysicsShapeType::CAPSULE:
            {
                float scaled_radius = shape_data.dimensions.x * (glm::max)(scale.x, scale.z);
                float scaled_half_height = shape_data.dimensions.y * scale.y;
                JPH::Ref<JPH::CapsuleShape> capsule_shape = new JPH::CapsuleShape(scaled_half_height, scaled_radius);
                new_shape = capsule_shape;
                break;
            }
        case PhysicsShapeType::MESH:
            {
                if (model == nullptr)
                {
                    Log::Warning("Cannot scale mesh collider: source model not available");
                    return;
                }

                const std::vector<Mesh>& meshes = model->meshes;
                if (meshes.empty())
                {
                    Log::Error("Cannot scale mesh collider: model has no meshes");
                    return;
                }

                JPH::VertexList vertices;
                JPH::IndexedTriangleList triangles;

                uint32_t vertex_offset = 0;
                uint32_t total_triangle_count = 0;

                for (const auto& mesh : meshes)
                {
                    for (const Vertex& vertex : mesh.vertices)
                    {
                        glm::vec3 scaled_position = vertex.position * scale;
                        vertices.push_back(JPH::Float3(scaled_position.x, scaled_position.y, scaled_position.z));
                    }

                    for (size_t i = 0; i < mesh.indices.size(); i += 3)
                    {
                        if (i + 2 < mesh.indices.size())
                        {
                            uint32_t idx0 = mesh.indices[i] + vertex_offset;
                            uint32_t idx1 = mesh.indices[i + 1] + vertex_offset;
                            uint32_t idx2 = mesh.indices[i + 2] + vertex_offset;

                            triangles.push_back(JPH::IndexedTriangle(idx0, idx1, idx2, 0));
                            total_triangle_count++;
                        }
                    }

                    vertex_offset += static_cast<uint32_t>(mesh.vertices.size());
                }

                if (vertices.empty() || triangles.empty())
                {
                    Log::Error("Cannot scale mesh collider: no valid geometry");
                    return;
                }

                JPH::MeshShapeSettings mesh_settings(vertices, triangles);
                JPH::Shape::ShapeResult result = mesh_settings.Create();

                if (result.IsValid())
                {
                    new_shape = result.Get();

                    shape_data.dimensions = glm::vec3(
                        static_cast<float>(vertices.size()),
                        static_cast<float>(total_triangle_count),
                        static_cast<float>(model->param_id)
                    );
                }
                else
                {
                    Log::Error("Failed to create scaled mesh shape: " + result.GetError());
                    return;
                }
                break;
            }
        }

        if (new_shape != nullptr)
        {
            body_interface.SetShape(body_id, new_shape.GetPtr(), true, JPH::EActivation::Activate);
        }
        else
        {
            Log::Error("Failed to scale physics body: new shape is null");
        }
    }

    glm::vec3 PhysicsBody::GetPosition() const
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 pos = body_interface.GetPosition(body_id);
        return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
    }

    glm::quat PhysicsBody::GetRotation() const
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Quat rot = body_interface.GetRotation(body_id);
        return glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());
    }

    void PhysicsBody::SetLinearVelocity(const glm::vec3& velocity)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetLinearVelocity(body_id, JPH::Vec3(velocity.x, velocity.y, velocity.z));
    }

    glm::vec3 PhysicsBody::GetLinearVelocity() const
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 vel = body_interface.GetLinearVelocity(body_id);
        return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsBody::SetAngularVelocity(const glm::vec3& angular_velocity)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetAngularVelocity(
            body_id, JPH::Vec3(angular_velocity.x, angular_velocity.y, angular_velocity.z));
    }

    glm::vec3 PhysicsBody::GetAngularVelocity() const
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        JPH::Vec3 vel = body_interface.GetAngularVelocity(body_id);
        return glm::vec3(vel.GetX(), vel.GetY(), vel.GetZ());
    }

    void PhysicsBody::AddForce(const glm::vec3& force)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddForce(body_id, JPH::Vec3(force.x, force.y, force.z));
    }

    void PhysicsBody::AddImpulse(const glm::vec3& impulse)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddImpulse(body_id, JPH::Vec3(impulse.x, impulse.y, impulse.z));
    }

    void PhysicsBody::AddTorque(const glm::vec3& torque)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.AddTorque(body_id, JPH::Vec3(torque.x, torque.y, torque.z));
    }

    void PhysicsBody::SetMass(float mass)
    {
        if (body_type != PhysicsBodyType::DYNAMIC || mass <= 0.0f) return;

        auto& physics_system = Engine::GetPhysicsManager();
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

        auto& physics_system = Engine::GetPhysicsManager();
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
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetFriction(body_id, friction);
    }

    void PhysicsBody::SetRestitution(float restitution)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetRestitution(body_id, restitution);
    }

    void PhysicsBody::SetGravityFactor(float factor)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        body_interface.SetGravityFactor(body_id, factor);
    }

    void PhysicsBody::SetEnabled(bool enabled)
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        if (enabled)
            body_interface.ActivateBody(body_id);
        else
            body_interface.DeactivateBody(body_id);
    }

    bool PhysicsBody::IsEnabled() const
    {
        auto& physics_system = Engine::GetPhysicsManager();
        JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
        return body_interface.IsActive(body_id);
    }
}
