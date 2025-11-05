#pragma once

namespace Bonfire
{

    class Camera
    {
    public:
        Camera(uint32_t id = 0, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, float fov = 60.0f);

        glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, up); }
        glm::mat4 GetProjectionMatrix(const float& width, const float& height) const { return glm::perspective(glm::radians(fov), width / height, 0.1f, 250.0f); }

        void LookAt(const glm::vec3& target);
        void UpdateCameraVectors();
        
        void SetWorldUpVector(const glm::vec3& world_up) { this->world_up = world_up; }
        
        glm::vec3& GetFrontVector() { return front; }
        glm::vec3& GetUpVector() { return up; }
        glm::vec3& GetRightVector() { return right; }
        glm::vec3& GetWorldUpVector() { return world_up; }

        bool& IsFirstMouse() { return first_mouse; }
        float& GetLastX() { return last_x; }
        float& GetLastY() { return last_y; }
        bool& IsOrthographic() { return is_orthographic; }

    public:
        uint32_t id;
        glm::vec3 position;
        float yaw;
        float pitch;
        float fov;
        
    private:
        bool is_orthographic = false;

        bool first_mouse = true;
        float last_x = 0.0f;
        float last_y = 0.0f;
        
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 world_up;
    };

}