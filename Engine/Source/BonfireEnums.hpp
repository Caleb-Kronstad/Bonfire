#pragma once

#include "iostream"

namespace Bonfire
{
    enum MOVEMENT_DIRECTION : std::uint8_t {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    enum TEXTURE_TYPE : std::uint8_t
    {
        NONE,
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };
}