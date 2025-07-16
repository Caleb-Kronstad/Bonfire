#pragma once

namespace Bonfire
{
    class Texture
    {
    public:
        Texture(std::string path, TEXTURE_TYPE type, bool flip = false);

        void Load();
        
    public:
        GLuint gl_id;
        TEXTURE_TYPE type;
        std::string path;
        bool flip;
    };
}
