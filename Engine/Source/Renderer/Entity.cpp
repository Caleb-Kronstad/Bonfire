#include "bonfire_pch.hpp"
#include "Entity.hpp"

namespace Bonfire
{

    Entity(const std::string& name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
        : name(name), position(position), rotation(rotation), scale(scale)
    {
        enabled = true;
    }

    glm::quat Entity::GetTransformOrientation()
    {
        return glm::quat(rotation / 180.0f * glm::pi<float>());
    }
    glm::mat4 Entity::GetTransformMatrix()
    {
        return glm::translate(glm::mat4(1.0f), position)
        * glm::toMat4(GetOrientation())
        * glm::scale(glm::mat4(1.0f), scale);
    }
}
