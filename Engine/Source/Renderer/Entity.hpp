#pragma once

#include "Shader.hpp"
#include "Components.hpp"
#include "ParamDatabase.hpp"
#include "Physics/PhysicsManager.hpp"
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
    template<>
    inline ComponentType GetComponentType<AudioComponent>() { return ComponentType::AUDIO; }
    template<>
    inline ComponentType GetComponentType<ScriptComponent>() { return ComponentType::SCRIPT; }
    template<>
    inline ComponentType GetComponentType<CameraComponent>() { return ComponentType::CAMERA; }
    
    class Entity
    {
    public:
        Entity(const uint32_t& id = 0, const bool& enabled = true, const std::string& name = "Entity", glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
            : id(id), enabled(enabled), name(name), position(position), rotation(rotation), scale(scale)
        {
        }

        void Draw(std::shared_ptr<Shader> shader, Scene& scene);
        
        void UpdateComponents();
        void ValidateDOFS();
        bool AddComponent(ComponentType type, std::shared_ptr<Component> component);
        bool RemoveComponent(ComponentType type);
        template<typename T> T& GetComponent()
        {
            assert(HasComponent<T>() && "Entity does not have component");
            return *std::static_pointer_cast<T>(components.at(GetComponentType<T>()));
        }
        template<typename T> bool HasComponent() const
        {
            return components.contains(GetComponentType<T>());
        }
        
        glm::quat GetTransformOrientation() const ;
        glm::mat4 GetTransformMatrix();
        glm::mat4 GetWorldTransformMatrix(const std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities);

        bool AddChild(uint32_t child_id);
        bool RemoveChild(uint32_t child_id);
        bool IsRoot() const { return parent == 0; }

        AABB GetWorldAABB(const std::unordered_map<uint32_t, std::shared_ptr<Entity>>& entities);

        glm::vec3 GetForwardVector() const;
        glm::vec3 GetRightVector() const;
        glm::vec3 GetUpVector() const;
        
        void SetPosition(const glm::vec3& new_position);
        void SetRotation(const glm::vec3& new_rotation);
        void SetScale(const glm::vec3& new_scale);

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
