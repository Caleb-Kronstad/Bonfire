#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "Material.hpp"

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
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
        ~Mesh();
        void Draw(Shader& shader, std::shared_ptr<Material> material);

    private:
        void Setup();

    public:
        GLuint vertex_buffer, element_buffer, vertex_array;
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
    };

}