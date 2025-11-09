#pragma once

#include "Core/Layer.hpp"
#include "PhysicsBody.hpp"
#include "Renderer/Model.hpp"

namespace Bonfire
{
    namespace Layers
    {
        static constexpr JPH::ObjectLayer NON_MOVING = 0;
        static constexpr JPH::ObjectLayer MOVING = 1;
        static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
    };
    namespace BroadPhaseLayers
    {
        static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
        static constexpr JPH::BroadPhaseLayer MOVING(1);
        static constexpr unsigned int NUM_LAYERS(2);
    };
    
    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl();
        virtual unsigned int GetNumBroadPhaseLayers() const override;
        virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;
    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
            virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
    #endif

    private:
        JPH::BroadPhaseLayer object_to_broad_phase[Layers::NUM_LAYERS];
    };

    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const override;
    };

    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer object1, JPH::ObjectLayer object2) const override;
    };

    class ContactListener : public JPH::ContactListener
    {
    public:
        virtual JPH::ValidateResult OnContactValidate(
            const JPH::Body &inBody1,
            const JPH::Body &inBody2,
            JPH::RVec3Arg inBaseOffset,
            const JPH::CollideShapeResult &inCollisionResult) override
        {
           // VALIDATE COLLISION
            return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        virtual void OnContactAdded(
            const JPH::Body &inBody1,
            const JPH::Body &inBody2,
            const JPH::ContactManifold &inManifold,
            JPH::ContactSettings &ioSettings) override
        {
            // COLLISION STARTED
        }

        virtual void OnContactPersisted(
            const JPH::Body &inBody1,
            const JPH::Body &inBody2,
            const JPH::ContactManifold &inManifold,
            JPH::ContactSettings &ioSettings) override
        {
            
        }

        virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
        {
            // COLLISION ENDED
        }
    };


    class PhysicsSystem : Layer
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem();
        
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(const float& delta_time) override;

        void SyncPhysicsToEntities();

        std::shared_ptr<PhysicsBody> CreateBoxBody(
            const glm::vec3& position,
            const glm::quat& rotation,
            const glm::vec3& half_extents,
            PhysicsBodyType body_type,
            float mass = 1.0f,
            float friction = 0.5f,
            float restitution = 0.0f
        );

        std::shared_ptr<PhysicsBody> CreateSphereBody(
            const glm::vec3& position,
            float radius,
            PhysicsBodyType body_type,
            float mass = 1.0f,
            float friction = 0.5f,
            float restitution = 0.0f
        );

        std::shared_ptr<PhysicsBody> CreateCapsuleBody(
            const glm::vec3& position,
            const glm::quat& rotation,
            float radius,
            float half_height,
            PhysicsBodyType body_type,
            float mass = 1.0f,
            float friction = 0.5f,
            float restitution = 0.0f
        );

        std::shared_ptr<PhysicsBody> CreateMeshBody(
            const glm::vec3& position,
            const glm::quat& rotation,
            std::shared_ptr<Model> model,
            uint32_t model_id,
            PhysicsBodyType body_type = PhysicsBodyType::STATIC,
            float mass = 1.0f,
            float friction = 0.5f,
            float restitution = 0.0f
            );

        void DestroyBody(std::shared_ptr<PhysicsBody> physics_object);
        void SetGravity(const glm::vec3& gravity);

        glm::vec3 GetGravity() const;
        JPH::BodyInterface& GetBodyInterface() { return jolt_physics_system->GetBodyInterface(); }
        const JPH::BodyInterface& GetBodyInterface() const { return jolt_physics_system->GetBodyInterface(); }
        const JPH::BodyLockInterface& GetBodyLockInterface() const { return jolt_physics_system->GetBodyLockInterface(); }

    private:
        std::unique_ptr<JPH::TempAllocatorImpl> temp_allocator;
        std::unique_ptr<JPH::JobSystemThreadPool> job_system;
        std::unique_ptr<BPLayerInterfaceImpl> broad_phase_layer_interface;
        std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> object_vs_broad_phase_layer_filter;
        std::unique_ptr<ObjectLayerPairFilterImpl> object_layer_pair_filter;
        std::unique_ptr<JPH::PhysicsSystem> jolt_physics_system;
        std::unique_ptr<ContactListener> contact_listener;

        const unsigned int max_bodies = 10240;
        const unsigned int num_body_mutexes = 0;
        const unsigned int max_body_pairs = 10240;
        const unsigned int max_contact_constraints = 10240;
    };
}
