#pragma once

#include "Shader.hpp"

namespace Bonfire
{
    class ShadowMap
    {
    public:
        void Generate(std::shared_ptr<Shader> point_shadow_map_shader, std::shared_ptr<Shader> lit_shader, const std::string& path);
        void Load(glm::vec3& light_pos);
        void Set(glm::vec3& light_pos);
        void Draw();
        void Reset(bool cull = false);

    public:
        bool updated_this_frame = false;
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
        unsigned int frame_buffer = 0, shadow_cubemap = 0, map_texture = 0;
        float aspect = 1.0f, near_plane = 1.0f, far_plane = 25.0f;

        glm::mat4 light_space_matrix;
        std::vector<glm::mat4> shadow_transforms;
        std::shared_ptr<Shader> point_shadow_map_shader, lit_shader;

    private:   
        static unsigned int LoadTexture(char const* path)
        {
            unsigned int textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << " ... (Debug in shadows.cpp file) ..." << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }
    };
}
