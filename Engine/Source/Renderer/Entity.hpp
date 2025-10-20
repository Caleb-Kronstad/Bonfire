#pragma once

#include "Shader.hpp"
#include "Components.hpp"
#include "ParamDatabase.hpp"

namespace Bonfire {

    class Scene;
    
    template<typename T>
    COMPONENT_TYPE GetComponentType();
    template<>
    inline COMPONENT_TYPE GetComponentType<ModelComponent>() { return COMPONENT_TYPE::MODEL; }
    template<>
    inline COMPONENT_TYPE GetComponentType<LightSourceComponent>() { return COMPONENT_TYPE::LIGHT; }
    template<>
    inline COMPONENT_TYPE GetComponentType<PhysicsComponent>() { return COMPONENT_TYPE::PHYSICS; }
    template<>
    inline COMPONENT_TYPE GetComponentType<AnimationComponent>() { return COMPONENT_TYPE::ANIMATION; }
    
    class Entity
    {
    public:
        Entity(const uint32_t& id = 0, const bool& enabled = true, const std::string& name = "Entity", glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f))
            : id(id), enabled(enabled), name(name), position(position), rotation(rotation), scale(scale)
        {
        }

        void Draw(std::shared_ptr<Shader> shader, Scene& scene, glm::mat4& manipulation_matrix, glm::mat4& view_matrix, glm::mat4& projection_matrix);
        
        bool AddComponent(COMPONENT_TYPE type, std::shared_ptr<Component> component);
        bool RemoveComponent(COMPONENT_TYPE type);
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
        std::unordered_map<COMPONENT_TYPE, std::shared_ptr<Component>> components;
    };

}
