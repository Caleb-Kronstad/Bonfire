#include "bonfire_pch.hpp"
#include "Mesh.hpp"

namespace Bonfire
{
    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<std::shared_ptr<Texture>> textures)
	    : vertices(vertices), indices(indices), textures(textures)
    {
        SetupMesh();
    }

    void Mesh::Draw(Shader& shader)
    {
		for (unsigned int i = 0; i < textures.size(); i++)
		{
		    glActiveTexture(GL_TEXTURE0+i);

			std::string texture_type_name = "diffuse";
			switch (textures[i]->type)
			{
			case DIFFUSE:
				{
					texture_type_name = "diffuse";
					break;
				}
			case SPECULAR:
				{
					texture_type_name = "specular";
					break;
				}
			case NORMAL:
				{
					texture_type_name = "normal";
					break;
				}
			case HEIGHT:
				{
					texture_type_name = "height";
					break;
				}
			default:
				break;
			}
			shader.SetInt("material."+texture_type_name, static_cast<int>(i));
			glBindTexture(GL_TEXTURE_2D, textures[i]->gl_id);
		}
        
        // bind and draw mesh
        glBindVertexArray(vertex_array);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

        // unbind
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    void Mesh::SetupMesh()
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

}