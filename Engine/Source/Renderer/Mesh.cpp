#include "bonfire_pch.hpp"
#include "Mesh.hpp"

namespace Bonfire
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
    }
    
}