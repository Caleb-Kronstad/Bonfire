#include "bonfire_pch.hpp"
#include "Camera.hpp"

namespace Bonfire
{
    Camera::Camera(unsigned int id, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : id(id), front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(5.0f), mouse_sensitivity(0.1f), fov(60.0f)
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

    void Camera::ProcessKeyboard(MovementDirection direction, float delta_time)
    {
        if (movement_disabled) return;
        
        float velocity = movement_speed * delta_time;
        if (direction == MovementDirection::FORWARD)
            position += front * velocity;
        if (direction == MovementDirection::BACKWARD)
            position -= front * velocity;
        if (direction == MovementDirection::LEFT)
            position -= right * velocity;
        if (direction == MovementDirection::RIGHT)
            position += right * velocity;
    }
    
    void Camera::ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch)
    {
        if (mouse_disabled) return;
        
        x_offset *= mouse_sensitivity;
        y_offset *= mouse_sensitivity;

        yaw += x_offset;
        pitch -= y_offset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch)
        {
            pitch = (std::max)(pitch, -89.0f);
            pitch = (std::min)(pitch, 89.0f);
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }
    
    void Camera::ProcessMouseScroll(float y_offset)
    {
        position += front * y_offset;
    }

    void Camera::UpdateCameraVectors()
    {
        // calculate the new front vector
        glm::vec3 Front = glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
        );
        front = glm::normalize(Front);
        // also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, front));
    }
}
