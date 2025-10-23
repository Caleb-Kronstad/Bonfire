#pragma once

#include "Shader.hpp"

namespace Bonfire
{
    class ShadowMap
    {
    public:
        ShadowMap(std::shared_ptr<Shader> point_shadow_map_shader, std::shared_ptr<Shader> shadow_map_shader, std::shared_ptr<Shader> lit_shader, const std::string& path);
        ~ShadowMap();
        void Load(glm::vec3& light_pos);
        void LoadDirectional(glm::vec3& light_dir);
        void SetDirectional();
        void Set(glm::vec3& light_pos);
        void Draw();
        void Reset(bool cull = false);
        GLuint LoadTexture(char const* path);

    public:
        bool updated_this_frame = false;
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        unsigned int frame_buffer = 0, shadow_cubemap = 0, map_texture = 0;
        unsigned int directional_frame_buffer = 0;
        unsigned int directional_shadow_map = 0;
        float aspect = 1.0f, near_plane = 1.0f, far_plane = 25.0f;

        glm::mat4 light_space_matrix;
        std::vector<glm::mat4> shadow_transforms;
        std::shared_ptr<Shader> point_shadow_map_shader, lit_shader, shadow_map_shader;

    private:   
        
    };
}
