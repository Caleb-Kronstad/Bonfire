#include "bonfire_pch.hpp"
#include "Entity.hpp"

namespace Bonfire
{
    Entity::Entity(std::string name)
        : name(name)
    {
        enabled = true;
        parent = nullptr;
        children = {};
    }

    void Entity::LoadComponents()
    {
        auto model = GetComponent<Model>();
        auto textures = GetComponent<Textures>();
        if (model)
        {
            model->Load(textures);
        }
    }
    
    void Entity::Draw(Shader& shader, glm::mat4& matrix)
    {
        if (GetComponent<Transform>() == nullptr)
            return;

        matrix = GetComponent<Transform>()->GetTransformMatrix();
        shader.SetMat4("model", matrix);
        
        auto model = GetComponent<Model>();
        if (model)
            model->Draw(shader);
    }

    template<typename T>
    std::shared_ptr<T> Entity::GetComponent()
    {
        COMPONENT_TYPE type = T::TYPE;
    
        if (components[type] != nullptr) {
            return std::static_pointer_cast<T>(components[type]);
        }
        return nullptr;
    }
}
