#pragma once

#include "bonfire_pch.hpp"
#include "Shader.hpp"

namespace Bonfire
{
    const std::array<float, 120> SKYBOX_VERTICES
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    class Skybox
    {
    public:
        Skybox(const std::string& faces_path = "S1", const std::array<float, 120>& vertices = SKYBOX_VERTICES)
            : skybox_shader(skybox_shader), faces_path(faces_path), vertices(vertices)
        {
            vertex_buffer = 0;
            vertex_array = 0;
            skybox_texture = 0;
            skybox_faces = {"px","nx","py","ny","pz","nz"};
        }
        ~Skybox();

        void Load(std::shared_ptr<Shader> skybox_shader, bool is_png = false);
        void Draw(const glm::mat4& camera_view_matrix, const glm::mat4& camera_projection_matrix);

    private:
        GLuint Bind(bool is_png);

    private:
		GLuint vertex_buffer, vertex_array;
        GLuint skybox_texture;
        std::string faces_path;
        std::array<float, 120> vertices;
        std::array<std::string, 6> skybox_faces;
        std::shared_ptr<Shader> skybox_shader;
    };
}