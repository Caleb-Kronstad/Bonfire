#include "bonfire_pch.hpp"
#include "Components.hpp"

namespace Bonfire
{
    // constructors
    
    Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        : position(position), rotation(rotation), scale(scale)
    {
    }

    ModelData::ModelData(std::shared_ptr<Model> model)
        : model(model)
    {
    }

    glm::quat Transform::GetOrientation()
    {
        return glm::quat(rotation / 180.0f * glm::pi<float>());
    }

    // other functions
    
    glm::mat4 Transform::GetTransformMatrix()
    {
        return glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(GetOrientation())
            * glm::scale(glm::mat4(1.0f), scale);
    }
}
