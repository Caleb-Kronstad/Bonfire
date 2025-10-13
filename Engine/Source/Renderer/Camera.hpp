#pragma once

namespace Bonfire
{

    class Camera
    {
    public:
        Camera(unsigned int id = 0, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

        glm::mat4 GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }
        glm::mat4 GetProjectionMatrix(const float& width, const float& height) const { return glm::perspective(glm::radians(Zoom), width / height, 0.1f, 250.0f); }
        
        void ProcessKeyboard(MOVEMENT_DIRECTION direction, float deltaTime);
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
        void ProcessMouseScroll(float yoffset);
        
    private:
        void UpdateCameraVectors();
        
    public:
        unsigned int id;
        bool isOrthographic = false;
        bool mouseLocked = false;

        // ALL SUBJECT TO CHANGE IN FUTURE UPDATES, MAINLY FOR TESTING THE ENGINE
        bool firstMouse = false;
        float lastX = 0.0f;
        float lastY = 0.0f;
        // --
        
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        float Yaw;
        float Pitch;
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

    };

}