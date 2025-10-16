#pragma once

#include "ParamDatabase.hpp"

namespace Bonfire
{
    class Texture
    {
    public:
        Texture(std::string path, TEXTURE_TYPE type, bool flip = false);

        void Load();

    public:
        uint32_t param_id;
        std::string name;
        GLuint gl_id;
        bool flip;
        TEXTURE_TYPE type;
        std::string path;
    };
}
