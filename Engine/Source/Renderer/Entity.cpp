#include "bonfire_pch.hpp"
#include "Entity.hpp"

namespace Bonfire
{
    Entity::Entity(std::string name)
        : name(name)
    {
        enabled = true;
    }

    void Entity::Draw(Shader& shader, glm::mat4& matrix)
    {
        if (GetComponent<Transform>() == nullptr)
            return;

        matrix = GetComponent<Transform>()->GetTransformMatrix();
        shader.SetMat4("model", matrix);
        
        auto model_data = GetComponent<ModelData>();
        if (model_data)
            model_data->model->Draw(shader);
    }
}
