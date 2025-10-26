#include "bonfire_pch.hpp"
#include "PhysicsSystem.hpp"

#include "Core/Project.hpp"

namespace Bonfire
{
    BPLayerInterfaceImpl::BPLayerInterfaceImpl()
    {
        object_to_broad_phase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        object_to_broad_phase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    unsigned int BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    JPH::BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
    {
        return object_to_broad_phase[layer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const
    {
        switch ((JPH::BroadPhaseLayer::Type)layer)
        {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
        default: return "INVALID";
        }
    }
#endif

    bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const
    {
        switch (layer1)
        {
        case Layers::NON_MOVING:
            return layer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
        }
    }

    bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer object1, JPH::ObjectLayer object2) const
    {
        switch (object1)
        {
        case Layers::NON_MOVING:
            return object2 == Layers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
        }
    }

    PhysicsSystem::PhysicsSystem()
        : Layer("PhysicsSystem")
    {
    }

    PhysicsSystem::~PhysicsSystem()
    {
    }

    void PhysicsSystem::OnAttach()
    {
        Log::Info("Initializing Jolt Physics...");

        JPH::RegisterDefaultAllocator();

        JPH::Trace = [](const char* fmt, ...) {
            va_list list;
            va_start(list, fmt);
            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), fmt, list);
            va_end(list);
            std::cout << buffer << std::endl;
        };

        temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

        job_system = std::make_unique<JPH::JobSystemThreadPool>(
            JPH::cMaxPhysicsJobs,
            JPH::cMaxPhysicsBarriers,
            std::thread::hardware_concurrency() - 1
        );

        JPH::Factory::sInstance = new JPH::Factory();

        JPH::RegisterTypes();

        broad_phase_layer_interface = std::make_unique<BPLayerInterfaceImpl>();
        object_vs_broad_phase_layer_filter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
        object_layer_pair_filter = std::make_unique<ObjectLayerPairFilterImpl>();

        jolt_physics_system = std::make_unique<JPH::PhysicsSystem>();
        jolt_physics_system->Init(
            max_bodies,
            num_body_mutexes,
            max_body_pairs,
            max_contact_constraints,
            *broad_phase_layer_interface,
            *object_vs_broad_phase_layer_filter,
            *object_layer_pair_filter
        );

        contact_listener = std::make_unique<ContactListener>();
        jolt_physics_system->SetContactListener(contact_listener.get());
        jolt_physics_system->SetGravity(JPH::Vec3(0.0f, -9.81f, 0.0f));

        Log::Info("Jolt Physics initialized successfully");
    }

    void PhysicsSystem::OnDetach()
    {
        Log::Info("Shutting down Jolt Physics...");

        jolt_physics_system.reset();
        object_layer_pair_filter.reset();
        object_vs_broad_phase_layer_filter.reset();
        broad_phase_layer_interface.reset();

        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;

        job_system.reset();
        temp_allocator.reset();

        Log::Info("Jolt Physics shut down successfully");
    }

    void PhysicsSystem::OnUpdate(const float& delta_time)
    {
        const int collision_steps = 1;
        jolt_physics_system->Update(delta_time, collision_steps, temp_allocator.get(), job_system.get());
        SyncPhysicsToEntities();
    }

    void PhysicsSystem::SyncPhysicsToEntities()
    {
        Scene& scene = Project::GetRenderer().GetScene();

        for (auto& [entity_id, entity] : scene.GetEntities())
        {
            if (entity->HasComponent<PhysicsComponent>())
            {
                PhysicsComponent& physics_component = entity->GetComponent<PhysicsComponent>();
                std::shared_ptr<PhysicsBody> physics_body = physics_component.physics_body;
                entity->position = physics_body->GetPosition();
                entity->rotation = glm::degrees(glm::eulerAngles(physics_body->GetRotation()));
            }
        }
    }

    std::shared_ptr<PhysicsBody> PhysicsSystem::CreateBoxBody(
        const glm::vec3& position,
        const glm::quat& rotation,
        const glm::vec3& half_extents,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        JPH::BoxShapeSettings shape_settings(JPH::Vec3(half_extents.x, half_extents.y, half_extents.z));
        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        JPH::ShapeRefC shape = shape_result.Get();

        JPH::EMotionType motion_type;
        JPH::ObjectLayer object_layer;

        switch (body_type)
        {
        case PhysicsBodyType::STATIC:
            motion_type = JPH::EMotionType::Static;
            object_layer = Layers::NON_MOVING;
            break;
        case PhysicsBodyType::KINEMATIC:
            motion_type = JPH::EMotionType::Kinematic;
            object_layer = Layers::MOVING;
            break;
        case PhysicsBodyType::DYNAMIC:
        default:
            motion_type = JPH::EMotionType::Dynamic;
            object_layer = Layers::MOVING;
            break;
        }

        JPH::BodyCreationSettings body_settings(
            shape,
            JPH::Vec3(position.x, position.y, position.z),
            JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
            motion_type,
            object_layer
        );

        body_settings.mFriction = friction;
        body_settings.mRestitution = restitution;
        if (motion_type == JPH::EMotionType::Dynamic)
            body_settings.mMassPropertiesOverride.mMass = mass;

        JPH::Body* body = jolt_physics_system->GetBodyInterface().CreateBody(body_settings);
        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::BOX;
        shape_data.dimensions = half_extents;

        return std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
    }

    std::shared_ptr<PhysicsBody> PhysicsSystem::CreateSphereBody(
        const glm::vec3& position,
        float radius,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        JPH::SphereShapeSettings shape_settings(radius);
        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        JPH::ShapeRefC shape = shape_result.Get();
        JPH::EMotionType motion_type;
        JPH::ObjectLayer object_layer;

        switch (body_type)
        {
        case PhysicsBodyType::STATIC:
            motion_type = JPH::EMotionType::Static;
            object_layer = Layers::NON_MOVING;
            break;
        case PhysicsBodyType::KINEMATIC:
            motion_type = JPH::EMotionType::Kinematic;
            object_layer = Layers::MOVING;
            break;
        case PhysicsBodyType::DYNAMIC:
        default:
            motion_type = JPH::EMotionType::Dynamic;
            object_layer = Layers::MOVING;
            break;
        }

        JPH::BodyCreationSettings body_settings(
            shape,
            JPH::Vec3(position.x, position.y, position.z),
            JPH::Quat::sIdentity(),
            motion_type,
            object_layer
        );

        body_settings.mFriction = friction;
        body_settings.mRestitution = restitution;
        if (motion_type == JPH::EMotionType::Dynamic)
            body_settings.mMassPropertiesOverride.mMass = mass;

        JPH::Body* body = jolt_physics_system->GetBodyInterface().CreateBody(body_settings);
        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::SPHERE;
        shape_data.dimensions = glm::vec3(radius, 0.0f, 0.0f);

        return std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
    }

    std::shared_ptr<PhysicsBody> PhysicsSystem::CreateCapsuleBody(
        const glm::vec3& position,
        const glm::quat& rotation,
        float radius,
        float half_height,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        JPH::CapsuleShapeSettings shape_settings(half_height, radius);
        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        JPH::ShapeRefC shape = shape_result.Get();

        JPH::EMotionType motion_type;
        JPH::ObjectLayer object_layer;

        switch (body_type)
        {
        case PhysicsBodyType::STATIC:
            motion_type = JPH::EMotionType::Static;
            object_layer = Layers::NON_MOVING;
            break;
        case PhysicsBodyType::KINEMATIC:
            motion_type = JPH::EMotionType::Kinematic;
            object_layer = Layers::MOVING;
            break;
        case PhysicsBodyType::DYNAMIC:
        default:
            motion_type = JPH::EMotionType::Dynamic;
            object_layer = Layers::MOVING;
            break;
        }

        JPH::BodyCreationSettings body_settings(
            shape,
            JPH::Vec3(position.x, position.y, position.z),
            JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
            motion_type,
            object_layer
        );

        body_settings.mFriction = friction;
        body_settings.mRestitution = restitution;
        if (motion_type == JPH::EMotionType::Dynamic)
            body_settings.mMassPropertiesOverride.mMass = mass;

        JPH::Body* body = jolt_physics_system->GetBodyInterface().CreateBody(body_settings);
        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::CAPSULE;
        shape_data.dimensions = glm::vec3(radius, half_height, 0.0f);

        return std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
    }
}