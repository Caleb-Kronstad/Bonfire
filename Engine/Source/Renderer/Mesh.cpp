#include "bonfire_pch.hpp"
#include "Mesh.hpp"

namespace Bonfire
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
	    : vertices(vertices), indices(indices)
    {
        Setup();
    }
	Mesh::~Mesh()
	{
    	glDeleteVertexArrays(1, &vertex_array);
    	glDeleteBuffers(1, &vertex_buffer);
    	glDeleteBuffers(1, &element_buffer);
	}
	
    void Mesh::Draw(Shader& shader, std::shared_ptr<Material> material)
    {
		for (size_t i = 0; i < TEXTURE_TYPE_COUNT; i++)
		{
			std::shared_ptr<Texture> texture = material->textures[i];
			unsigned int texture_unit = 0;
			std::string texture_type_name = "diffuse";
			switch (static_cast<TextureType>(i))
			{
			case TextureType::DIFFUSE:
				texture_unit = 0;
				texture_type_name = "diffuse";
				break;
			case TextureType::SPECULAR:
				texture_unit = 1;
				texture_type_name = "specular";
				break;
			case TextureType::NORMAL:
				texture_unit = 2;
				texture_type_name = "normal";
				break;
			case TextureType::HEIGHT:
				texture_unit = 3;
				texture_type_name = "height";
				break;
			case TextureType::EMISSION:
				texture_unit = 4;
				texture_type_name = "emission";
				break;
			}
			glActiveTexture(GL_TEXTURE0 + texture_unit);
			shader.SetInt("material." + texture_type_name, static_cast<int>(texture_unit));
			glBindTexture(GL_TEXTURE_2D, texture->gl_id);
		}

    	glBindVertexArray(vertex_array);
    	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

    	glBindVertexArray(0);
    	glActiveTexture(GL_TEXTURE0);
    }

    void Mesh::Setup()
    {
        glGenVertexArrays(1, &vertex_array);
        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &element_buffer);

        glBindVertexArray(vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        // normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);
        // texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
        
		glBindVertexArray(0);
    }

	Mesh::Mesh(Mesh&& other) noexcept
	  : vertices(std::move(other.vertices)),
		indices(std::move(other.indices)),
		vertex_array(other.vertex_array),
		vertex_buffer(other.vertex_buffer),
		element_buffer(other.element_buffer)
    {
    	other.vertex_array = 0;
    	other.vertex_buffer = 0;
    	other.element_buffer = 0;
    }

	Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
    	if (this != &other)
    	{
    		glDeleteVertexArrays(1, &vertex_array);
    		glDeleteBuffers(1, &vertex_buffer);
    		glDeleteBuffers(1, &element_buffer);

    		vertices = std::move(other.vertices);
    		indices = std::move(other.indices);
    		vertex_array = other.vertex_array;
    		vertex_buffer = other.vertex_buffer;
    		element_buffer = other.element_buffer;

    		other.vertex_array = 0;
    		other.vertex_buffer = 0;
    		other.element_buffer = 0;
    	}
    	return *this;
    }

}