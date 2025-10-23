#pragma once

#include "ParamDatabase.hpp"

namespace Bonfire
{
    class Texture
    {
    public:
        Texture(std::string path, TextureType type, bool flip = false);
        ~Texture();

        void Load();

    public:
        uint32_t param_id;
        std::string name;
        GLuint gl_id;
        bool flip;
        TextureType type;
        std::string path;
    };
}
