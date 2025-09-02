#include "bonfire_pch.hpp"
#include "Entity.hpp"

namespace Bonfire
{
    Entity::Entity(std::string name)
        : name(name)
    {
        enabled = true;
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
}
