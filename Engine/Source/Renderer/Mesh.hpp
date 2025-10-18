#pragma once

#include "Shader.hpp"
#include "Texture.hpp"

namespace Bonfire
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
    };
    
    class Mesh
    {
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
        void Draw(Shader& shader, const std::vector<std::shared_ptr<Texture>>& textures);

    private:
        void Setup();

    public:
        GLuint vertex_buffer, element_buffer, vertex_array;
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
    };

}