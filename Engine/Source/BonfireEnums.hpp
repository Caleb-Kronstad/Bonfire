#pragma once

#include "iostream"

namespace Bonfire
{
    enum class ComponentType : std::uint8_t
    {
        UNKNOWN,
        MODEL,
        LIGHT,
        PHYSICS,
        ANIMATION,
        COUNT
    };

    enum class TextureType : std::uint8_t
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };

    enum class PhysicsBodyType : std::uint8_t
    {
        STATIC,
        DYNAMIC,
        KINEMATIC
    };

    enum class PhysicsShapeType : std::uint8_t
    {
        BOX,
        SPHERE,
        CAPSULE
    };

    enum class DebugType : std::uint8_t
    {
        DEFAULT,
        WIREFRAME,
        POINT
    };
}
