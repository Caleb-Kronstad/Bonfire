#pragma once

#include "Shader.hpp"
#include "Components.hpp"
#include "ParamDatabase.hpp"
#include "Physics/PhysicsSystem.hpp"
#include "Camera.hpp"

namespace Bonfire {

    class Scene;
    
    template<typename T>
    ComponentType GetComponentType();
    template<>
    inline ComponentType GetComponentType<ModelComponent>() { return ComponentType::MODEL; }
    template<>
    inline ComponentType GetComponentType<LightSourceComponent>() { return ComponentType::LIGHT; }
    template<>
    inline ComponentType GetComponentType<PhysicsComponent>() { return ComponentType::PHYSICS; }
    template<>
    inline ComponentType GetComponentType<AnimationComponent>() { return ComponentType::ANIMATION; }
    
    class Entity
    {
    public:
        Entity(const uint32_t& id = 0, const bool& enabled = true, const std::string& name = "Entity", glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
            : id(id), enabled(enabled), name(name), position(position), rotation(rotation), scale(scale)
        {
        }

        void Draw(Camera& camera, std::shared_ptr<Shader> shader, Scene& scene, glm::mat4& manipulation_matrix, glm::mat4& view_matrix, glm::mat4& projection_matrix);
        void UpdateComponents(PhysicsSystem& physics_system);
        
        bool AddComponent(ComponentType type, std::shared_ptr<Component> component);
        bool RemoveComponent(ComponentType type);
        template<typename T> T& GetComponent() { return *std::static_pointer_cast<T>(components.at(GetComponentType<T>())); }
        template<typename T> bool HasComponent() const { return components.contains(GetComponentType<T>()); }
        
        glm::quat GetTransformOrientation();
        glm::mat4 GetTransformMatrix();
        glm::mat4 GetWorldTransformMatrix(const std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities);

        bool AddChild(uint32_t child_id);
        bool RemoveChild(uint32_t child_id);
        bool IsRoot() const { return parent == 0; }

        AABB GetWorldAABB(const std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities);

    public:
        uint32_t id;
        bool enabled;
        std::string name;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        uint32_t parent = 0;
        std::vector<uint32_t> children;

    private:
        std::unordered_map<ComponentType, std::shared_ptr<Component>> components;
    };

}
