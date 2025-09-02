#ifndef BONFIRE_ENTITY_HPP
#define BONFIRE_ENTITY_HPP

#include "Shader.hpp"
#include "Components.hpp"

namespace Bonfire
{
    class Entity
    {
    public:
        Entity(std::string name = "NewEntity");

        void Draw(Shader& shader, glm::mat4& matrix);
        void LoadComponents();

        template<typename T, typename... Args>
        bool AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

            COMPONENT_TYPE type = T::TYPE;

            if (components[type] != nullptr) {
                Log::Warning("[FAILED] Component already added to Entity");
                return false;
            }

            components[type] = std::make_shared<T>(std::forward<Args>(args)...);
            return true;
        }
        bool RemoveComponent(COMPONENT_TYPE type)
        {
            if (components[type] != nullptr) {
                components[type] = nullptr;
                return true;
            }
    
            Log::Warning("[FAILED] Entity does not contain this Component");
            return false;
        }
        template<typename T>
        std::shared_ptr<T> GetComponent()
        {
            COMPONENT_TYPE type = T::TYPE;
    
            if (components[type] != nullptr) {
                return std::static_pointer_cast<T>(components[type]);
            }
            return nullptr;
        }

    public:
        std::string name;
        bool enabled;

        std::array<std::shared_ptr<Component>, COMPONENT_TYPE::COUNT> components;

    };
}

#endif