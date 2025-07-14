#pragma once
#include "Shader.hpp"

namespace Bonfire
{
    struct Texture {
        unsigned int ID;
        std::string Type;
        std::string Path;
    };

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    
    class Mesh
    {
    private:
        GLuint VBO, EBO, VAO;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;
        
    public:

        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
        //void Draw(Shader& shader);
    };

}
