#include "bonfire_pch.hpp"
#include "Texture.hpp"

namespace Bonfire
{
    Texture::Texture(std::string path, TEXTURE_TYPE type, bool flip)
        : path(path), type(type), flip(flip)
    {
        gl_id = 0;
    }

    void Texture::Load()
    {
        stbi_set_flip_vertically_on_load(flip);

        glGenTextures(1, &gl_id);
        glBindTexture(GL_TEXTURE_2D, gl_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
                Log::Warning("Unexpected nrComponents = " + std::to_string(nrComponents) + ", defaulting to GL_RGB\n");

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
            Log::Error("Texture failed to load at path: " + std::string(path) + "\n");
        stbi_image_free(data);
    }
}