#include "bonfire_pch.hpp"
#include "Skybox.hpp"

namespace Bonfire
{
    Skybox::~Skybox()
    {
        if (vertex_array != 0)
            glDeleteVertexArrays(1, &vertex_array);
        if (vertex_buffer != 0)
            glDeleteBuffers(1, &vertex_buffer);
        if (skybox_texture != 0)
            glDeleteTextures(1, &skybox_texture);
    }
    
    void Skybox::Load(std::shared_ptr<Shader> skybox_shader, bool is_png)
    {
        this->skybox_shader = skybox_shader;
        skybox_shader->Use();
        skybox_shader->SetInt("skybox", 0);
        
        glGenVertexArrays(1, &vertex_array);
        glGenBuffers(1, &vertex_buffer);
        glBindVertexArray(vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        skybox_texture = Bind(is_png);
    }

    void Skybox::Draw(const glm::mat4& camera_view_matrix, const glm::mat4& camera_projection_matrix, Fog& fog)
    {
        glDepthFunc(GL_LEQUAL);

        skybox_shader->Use();
        skybox_shader->SetMat4("view", glm::mat4(glm::mat3(camera_view_matrix)));
        skybox_shader->SetMat4("projection", camera_projection_matrix);

        fog.ApplyToShader(*skybox_shader);
        
        glBindVertexArray(vertex_array);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    GLuint Skybox::Bind(bool is_png)
    {
        stbi_set_flip_vertically_on_load(false);
        
        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        int width, height, nr_components;
        for (unsigned int i = 0; i < 6; i++)
        {
            GLenum format;
            GLint internal_format;
            
            std::string extension = is_png ? ".png" : ".jpg";
            std::string path = faces_path + "/" + skybox_faces.at(i) + extension;
            
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_components, 0);
            if (data)
            {
                if (nr_components == 3)
                {
                    format = GL_RGB;
                    internal_format = GL_RGB8;
                }
                else if (nr_components == 4)
                {
                    format = GL_RGBA;
                    internal_format = GL_RGBA8;
                }
                else
                {
                    Log::Error("Unsupported image type");
                    return 0;
                }
                
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                Log::Error("Failed to load Skybox at path: " + path);
                stbi_image_free(data);
                return 0;
            }
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture_id;
    }
}