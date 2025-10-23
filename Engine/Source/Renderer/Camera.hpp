#pragma once

namespace Bonfire
{

    class Camera
    {
    public:
        Camera(unsigned int id = 0, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

        glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, up); }
        glm::mat4 GetProjectionMatrix(const float& width, const float& height) const { return glm::perspective(glm::radians(fov), width / height, 0.1f, 250.0f); }

        void LookAt(const glm::vec3& target);
        
        void ProcessKeyboard(MovementDirection direction, float delta_time);
        void ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch = true);
        void ProcessMouseScroll(float y_offset);
        
    private:
        void UpdateCameraVectors();
        
    public:
        unsigned int id;
        bool is_orthographic = false;
        bool mouse_locked = false;
        bool movement_disabled = false;
        bool mouse_disabled = true;

        // ALL SUBJECT TO CHANGE IN FUTURE UPDATES, MAINLY FOR TESTING THE ENGINE
        bool first_mouse = false;
        float last_x = 0.0f;
        float last_y = 0.0f;
        // --
        
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;

        float yaw;
        float pitch;
        float movement_speed;
        float mouse_sensitivity;
        float fov;
    };

}