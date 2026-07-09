#include "bonfire_pch.hpp"
#include "Texture.hpp"

namespace Bonfire
{
    static void FlipImageVertically(unsigned char* data, int width, int height, int channels)
    {
        int stride = width * channels;
        std::vector<unsigned char> row(stride);
        for (int y = 0; y < height / 2; y++)
        {
            unsigned char* top = data + y * stride;
            unsigned char* bottom = data + (height - 1 - y) * stride;
            std::memcpy(row.data(), top, stride);
            std::memcpy(top, bottom, stride);
            std::memcpy(bottom, row.data(), stride);
        }
    }

    Texture::Texture(std::string path, TextureType type, bool flip)
        : path(path), type(type), flip(flip)
    {
        gl_id = 0;
    }
    Texture::~Texture()
    {
        if (gl_id != 0)
            glDeleteTextures(1, &gl_id);
    }

    void Texture::Load()
    {
        glGenTextures(1, &gl_id);
        glBindTexture(GL_TEXTURE_2D, gl_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrComponents;
        unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);
        if (data)
        {
            if (flip)
                FlipImageVertically(data, width, height, nrComponents);

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
            Log::Error("Texture failed to load at path: " + std::string(path) + " - " + SOIL_last_result() + "\n");
        free(data);
    }
}