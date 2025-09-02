#ifndef BONFIRE_ENUMS_HPP
#define BONFIRE_ENUMS_HPP

#include "iostream"

namespace Bonfire
{
    enum MOVEMENT_DIRECTION : std::uint8_t
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    enum TEXTURE_TYPE : std::uint8_t
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };

    enum COMPONENT_TYPE : std::uint8_t
    {
        TRANSFORM,
        MODEL,
        TEXTURES,
        COUNT // used to count how many component types there are
    };
}

#endif