#include "bonfire_pch.hpp"
#include "PhysicsManager.hpp"

#include "Core/Engine.hpp"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"

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

    PhysicsManager::PhysicsManager()
        : Layer("PhysicsManager")
    {
    }

    PhysicsManager::~PhysicsManager()
    {
    }

    void PhysicsManager::OnAttach()
    {
        Log::Info("Initializing Jolt Physics...");
        
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();

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
        
        lock.unlock();

        Log::Info("Jolt Physics initialized successfully");
    }

    void PhysicsManager::OnDetach()
    {
        Log::Info("Shutting down Jolt Physics...");
        
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();

        jolt_physics_system.reset();
        object_layer_pair_filter.reset();
        object_vs_broad_phase_layer_filter.reset();
        broad_phase_layer_interface.reset();

        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;

        job_system.reset();
        temp_allocator.reset();
        
        lock.unlock();

        Log::Info("Jolt Physics shut down successfully");
    }

    void PhysicsManager::OnUpdate(const float& delta_time)
    {
        constexpr int collision_steps = 1;
        jolt_physics_system->Update(delta_time, collision_steps, temp_allocator.get(), job_system.get());
    }

    std::shared_ptr<PhysicsBody> PhysicsManager::CreateBoxBody(
        const glm::vec3& position,
        const glm::quat& rotation,
        const glm::vec3& half_extents,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        
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
        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::DontActivate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::BOX;
        shape_data.dimensions = half_extents;

        lock.unlock();
        return std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
    }

    std::shared_ptr<PhysicsBody> PhysicsManager::CreateSphereBody(
        const glm::vec3& position,
        float radius,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        
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
        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::DontActivate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::SPHERE;
        shape_data.dimensions = glm::vec3(radius, 0.0f, 0.0f);

        lock.unlock();
        return std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
    }

    std::shared_ptr<PhysicsBody> PhysicsManager::CreateCapsuleBody(
        const glm::vec3& position,
        const glm::quat& rotation,
        float radius,
        float half_height,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        
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
        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::DontActivate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::CAPSULE;
        shape_data.dimensions = glm::vec3(radius, half_height, 0.0f);

        lock.unlock();
        return std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
    }

    std::shared_ptr<PhysicsBody> PhysicsManager::CreateMeshBody(
        const glm::vec3& position,
        const glm::quat& rotation,
        std::shared_ptr<Model> model,
        uint32_t model_id,
        PhysicsBodyType body_type,
        float mass,
        float friction,
        float restitution)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        
        if (!model)
        {
            Log::Error("Cannot create mesh collider: no model provided");
            return nullptr;
        }

        if (body_type != PhysicsBodyType::STATIC)
        {
            Log::Error("Mesh colliders must be static");
            return nullptr;
        }

        JPH::VertexList vertices;
        JPH::IndexedTriangleList triangles;

        uint32_t vertex_offset = 0;
        uint32_t total_triangle_count = 0;

        for (const Mesh& mesh : model->meshes)
        {
            for (const Vertex& vertex : mesh.vertices)
                vertices.push_back(JPH::Float3(vertex.position.x, vertex.position.y, vertex.position.z));

            for (size_t i = 0; i < mesh.indices.size(); i += 3)
            {
                if (i + 2 < mesh.indices.size())
                {
                    uint32_t idx0 = mesh.indices[i] + vertex_offset;
                    uint32_t idx1 = mesh.indices[i+1] + vertex_offset;
                    uint32_t idx2 = mesh.indices[i+2] + vertex_offset;

                    triangles.push_back(JPH::IndexedTriangle(idx0, idx1, idx2, 0));
                    total_triangle_count++;
                }
            }

            vertex_offset += static_cast<uint32_t>(mesh.vertices.size());
        }

        if (total_triangle_count > 1000)
            Log::Warning("Mesh collider has " + std::to_string(total_triangle_count) + " triangles - Consider using a simplified collision mesh for better performance");

        JPH::MeshShapeSettings mesh_settings(vertices, triangles);
        JPH::ShapeSettings::ShapeResult mesh_result = mesh_settings.Create();
        if (mesh_result.HasError())
        {
            Log::Error("Failed to create mesh shape: " + std::string(mesh_result.GetError().c_str()));
            return nullptr;
        }

        JPH::ScaledShapeSettings scaled_settings(mesh_result.Get(), JPH::Vec3(1.0f, 1.0f, 1.0f));
        JPH::ShapeSettings::ShapeResult shape_result = scaled_settings.Create();
        if (shape_result.HasError())
        {
            Log::Error("Failed to create scaled mesh shape: " + std::string(shape_result.GetError().c_str()));
            return nullptr;
        }

        JPH::ShapeRefC shape = shape_result.Get();
        JPH::EMotionType motion_type = JPH::EMotionType::Static;
        JPH::ObjectLayer object_layer = Layers::NON_MOVING;

        JPH::BodyCreationSettings body_settings(
            shape,
            JPH::Vec3(position.x, position.y, position.z),
            JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
            motion_type,
            object_layer
        );

        body_settings.mFriction = friction;
        body_settings.mRestitution = restitution;

        JPH::Body* body = jolt_physics_system->GetBodyInterface().CreateBody(body_settings);
        if (!body)
        {
            Log::Error("Failed to create physics body for mesh collider");
            return nullptr;
        }

        jolt_physics_system->GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::DontActivate);

        PhysicsShapeData shape_data;
        shape_data.type = PhysicsShapeType::MESH;
        shape_data.dimensions = glm::vec3(
            static_cast<float>(vertices.size()),  
            static_cast<float>(total_triangle_count),  
            static_cast<float>(model_id)
        );

        std::shared_ptr<PhysicsBody> physics_body = std::make_shared<PhysicsBody>(body->GetID(), body_type, shape_data);
        physics_body->mesh_id = model_id;

        lock.unlock();
        return physics_body;
    }

    void PhysicsManager::RegisterBodyEntity(JPH::BodyID body_id, std::shared_ptr<Entity> entity)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        body_to_entity_map.insert_or_assign(body_id, entity);
        lock.unlock();
    }
    void PhysicsManager::UnregisterBodyEntity(JPH::BodyID body_id)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        body_to_entity_map.erase(body_id);
        lock.unlock();
    }
    std::shared_ptr<Entity> PhysicsManager::GetEntityFromBodyId(JPH::BodyID body_id)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        auto it = body_to_entity_map.find(body_id);
        if (it != body_to_entity_map.end())
        {
            lock.unlock();
            return it->second;
        }
        lock.unlock();
        return nullptr;
    }
    
    JPH::Ref<JPH::GroupFilterTable> PhysicsManager::Filter(const std::string& filter_name, uint32_t num_subgroups)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        if (collision_filters.contains(filter_name)) return collision_filters.at(filter_name);
        JPH::Ref<JPH::GroupFilterTable> filter = new JPH::GroupFilterTable(num_subgroups);
        collision_filters.insert_or_assign(filter_name, filter);
        lock.unlock();
        return filter;
    }

    // COLLISION HANDLING

    static uint64_t MakeCollisionPairKey(JPH::BodyID body1, JPH::BodyID body2)
    {
        uint32_t id1 = body1.GetIndexAndSequenceNumber();
        uint32_t id2 = body2.GetIndexAndSequenceNumber();
        if (id1 > id2) std::swap(id1, id2);
        return (static_cast<uint64_t>(id1) << 32) | id2;
    }

    void PhysicsManager::OnCollisionEnter(JPH::BodyID body1, JPH::BodyID body2)
    {
        uint64_t pair_key = MakeCollisionPairKey(body1, body2);
        active_collision_pairs.insert(pair_key);
    }

    void PhysicsManager::OnCollisionConstant(JPH::BodyID body1, JPH::BodyID body2)
    {
        
    }

    void PhysicsManager::OnCollisionExit(JPH::BodyID body1, JPH::BodyID body2)
    {
        uint64_t pair_key = MakeCollisionPairKey(body1, body2);
        active_collision_pairs.erase(pair_key);
    }

    bool PhysicsManager::AreBodiesColliding(JPH::BodyID body1, JPH::BodyID body2)
    {
        std::unique_lock<std::mutex> lock = Engine::GetThreadManager().LockPhysicsMutex();
        uint64_t pair_key = MakeCollisionPairKey(body1, body2);
        lock.unlock();
        return active_collision_pairs.contains(pair_key);
    }

    void ContactListener::OnContactAdded(
           const JPH::Body &inBody1,
           const JPH::Body &inBody2,
           const JPH::ContactManifold &inManifold,
           JPH::ContactSettings &ioSettings)
    {
        JPH::BodyID body1_id = inBody1.GetID();
        JPH::BodyID body2_id = inBody2.GetID();
        Engine::GetPhysicsManager().OnCollisionEnter(body1_id, body2_id);
    }

    void ContactListener::OnContactPersisted(
        const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& inManifold,
        JPH::ContactSettings& ioSettings)
    {
        JPH::BodyID body1_id = inBody1.GetID();
        JPH::BodyID body2_id = inBody2.GetID();
        Engine::GetPhysicsManager().OnCollisionConstant(body1_id, body2_id);
    }

    void ContactListener::OnContactRemoved(
        const JPH::SubShapeIDPair& inSubShapePair)
    {
        JPH::BodyID body1_id = inSubShapePair.GetBody1ID();
        JPH::BodyID body2_id = inSubShapePair.GetBody2ID();
        Engine::GetPhysicsManager().OnCollisionExit(body1_id, body2_id);
    }
}