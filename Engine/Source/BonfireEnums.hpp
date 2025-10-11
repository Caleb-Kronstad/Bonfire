#pragma once

#include "iostream"

namespace Bonfire
{
    enum class PARAM_TYPE : std::uint8_t
    {
        MODEL,
        TEXTURE,
        AI,
        PHYSICS,
        ANIMATION,
        LOOT
    };

    enum class MOVEMENT_DIRECTION : std::uint8_t
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    enum class TEXTURE_TYPE : std::uint8_t
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };
}
