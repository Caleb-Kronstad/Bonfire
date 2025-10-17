#pragma once

namespace Bonfire
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;

        Ray(const glm::vec3& origin, const glm::vec3& direction)
            : origin(origin), direction(direction) {}
    };

    struct AABB
    {
        glm::vec3 minimum;
        glm::vec3 maximum;

        AABB() : minimum(glm::vec3(FLT_MAX)), maximum(glm::vec3(-FLT_MAX)) {}
        AABB(const glm::vec3& min, const glm::vec3& max)
            : minimum(min), maximum(max) {}

        void Expand(const glm::vec3& point)
        {
            minimum = (glm::min)(minimum, point);
            maximum = (glm::max)(maximum, point);
        }

        glm::vec3 GetCenter() const { return (minimum + maximum) * 0.5f; }
        glm::vec3 GetExtents() const { return (maximum - minimum) * 0.5f; }
    };

    Ray ScreenPointToRay(const glm::vec2& mouse_pos, const glm::vec2& viewport_min, const glm::vec2& viewport_size, const glm::mat4& view_matrix, const glm::mat4& projection_matrix);
    bool RayIntersectsAABB(const Ray& ray, const AABB& aabb, float& t_min);
    AABB TransformAABB(const AABB& aabb, const glm::mat4& transform);
}
