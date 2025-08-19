#include "bonfire_pch.hpp"
#include "Transform.hpp"

namespace Bonfire
{
    Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        : position(position), rotation(rotation), scale(scale)
    {
    }

}