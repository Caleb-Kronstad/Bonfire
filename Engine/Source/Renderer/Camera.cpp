#include "bonfire_pch.hpp"
#include "Camera.hpp"

namespace Bonfire
{
    Camera::Camera(uint32_t id, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : id(id), front(glm::vec3(0.0f, 0.0f, -1.0f)), fov(60.0f)
    {
        this->position = position;
        this->world_up = up;
        this->yaw = yaw;
        this->pitch = pitch;

        UpdateCameraVectors();
    }

    void Camera::LookAt(const glm::vec3& target)
    {
        glm::vec3 direction = glm::normalize(target - position);
        yaw = glm::degrees(atan2(direction.z, direction.x));
        pitch = glm::degrees(asin(direction.y));

        UpdateCameraVectors();
    }

    void Camera::UpdateCameraVectors()
    {
        // calculate the new front vector
        glm::vec3 new_front = glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        );
        front = glm::normalize(new_front);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
}
