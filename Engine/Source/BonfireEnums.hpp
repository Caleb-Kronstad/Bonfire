#pragma once

#include "iostream"

namespace Bonfire
{
    enum class COMPONENT_TYPE : std::uint8_t // also used for params
    {
        UNKNOWN,
        MODEL,
        PHYSICS,
        ANIMATION,
        COUNT
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
