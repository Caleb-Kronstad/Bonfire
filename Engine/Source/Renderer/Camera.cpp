#include "bonfire_pch.hpp"
#include "Camera.hpp"

namespace Bonfire
{
    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.1f), Zoom(60.0f)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        UpdateCameraVectors();
    }
    Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(5.0f), MouseSensitivity(0.1f), Zoom(60.0f)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        
        Pitch = pitch;
        UpdateCameraVectors();
    }

    void Camera::ProcessKeyboard(MOVEMENT_DIRECTION direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == MOVEMENT_DIRECTION::FORWARD)
            Position += Front * velocity;
        if (direction == MOVEMENT_DIRECTION::BACKWARD)
            Position -= Front * velocity;
        if (direction == MOVEMENT_DIRECTION::LEFT)
            Position -= Right * velocity;
        if (direction == MOVEMENT_DIRECTION::RIGHT)
            Position += Right * velocity;
    }
    
    void Camera::ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrainPitch)
    {
        x_offset *= MouseSensitivity;
        y_offset *= MouseSensitivity;

        Yaw += x_offset;
        Pitch -= y_offset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }
    
    void Camera::ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 15.0f)
            Zoom = 15.0f;
        if (Zoom > 90.0f)
            Zoom = 90.0f;
    }

    void Camera::UpdateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front = glm::vec3(
            cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
            sin(glm::radians(Pitch)),
            sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))
        );
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
}
