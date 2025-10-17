#include "bonfire_pch.hpp"
#include "Ray.hpp"

namespace Bonfire
{
    Ray ScreenPointToRay(const glm::vec2& mouse_pos, const glm::vec2& viewport_min, const glm::vec2& viewport_size, const glm::mat4& view_matrix, const glm::mat4& projection_matrix)
    {
        glm::vec2 relative_pos = mouse_pos - viewport_min;
        float x = (2.0f * relative_pos.x) / viewport_size.x - 1.0f;
        
        float y = 1.0f - (2.0f * relative_pos.y) / viewport_size.y; // flip y because opengl
        glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::mat4 inv_projection = glm::inverse(projection_matrix);
        glm::vec4 ray_eye = inv_projection * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        glm::mat4 inv_view = glm::inverse(view_matrix);
        glm::vec4 ray_world = inv_view * ray_eye;
        glm::vec3 ray_direction = glm::normalize(glm::vec3(ray_world));

        glm::vec3 ray_origin = glm::vec3(inv_view[3]);
        return Ray(ray_origin, ray_direction);
    }

    bool RayIntersectsAABB(const Ray& ray, const AABB& aabb, float& t_min)
    {
        glm::vec3 inv_dir = 1.0f / ray.direction;
        glm::vec3 t0 = (aabb.minimum - ray.origin) * inv_dir;
        glm::vec3 t1 = (aabb.maximum - ray.origin) * inv_dir;
        glm::vec3 t_min_vec = (glm::min)(t0, t1);
        glm::vec3 t_max_vec = (glm::max)(t0, t1);

        float t_near = (glm::max)((glm::max)(t_min_vec.x, t_min_vec.y), t_min_vec.z);
        float t_far = (glm::min)((glm::min)(t_max_vec.x, t_max_vec.y), t_max_vec.z);

        if (t_near > t_far || t_far < 0.0f)
            return false;
        t_min = t_near > 0.0f ? t_near : t_far;
        return true;
    }

    AABB TransformAABB(const AABB& aabb, const glm::mat4& transform)
    {
        glm::vec3 corners[8] = {
            glm::vec3(aabb.minimum.x, aabb.minimum.y, aabb.minimum.z),
            glm::vec3(aabb.minimum.x, aabb.minimum.y, aabb.maximum.z),
            glm::vec3(aabb.minimum.x, aabb.maximum.y, aabb.minimum.z),
            glm::vec3(aabb.minimum.x, aabb.maximum.y, aabb.maximum.z),
            glm::vec3(aabb.maximum.x, aabb.minimum.y, aabb.minimum.z),
            glm::vec3(aabb.maximum.x, aabb.minimum.y, aabb.maximum.z),
            glm::vec3(aabb.maximum.x, aabb.maximum.y, aabb.minimum.z),
            glm::vec3(aabb.maximum.x, aabb.maximum.y, aabb.maximum.z)
        };
        AABB transformed_aabb;
        for (int i = 0; i < 8; i++)
        {
            glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
            transformed_aabb.Expand(glm::vec3(transformed));
        }
        return transformed_aabb;
    }
}
