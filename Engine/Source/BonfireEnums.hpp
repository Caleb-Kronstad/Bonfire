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
        AUDIO,
        SCRIPT,
        CAMERA
    };

    inline constexpr uint8_t TEXTURE_TYPE_COUNT = 5;
    enum class TextureType : std::uint8_t
    {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT,
        EMISSION
    };
    
    enum class AnimationState : std::uint8_t
    {
        STOPPED,
        PLAYING,
        PAUSED
    };

    enum class FogType : std::uint8_t
    {
        LINEAR = 0,
        EXPONTENTIAL = 1,
        EXPONENTIAL_SQUARED = 2
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
        CAPSULE,
        MESH
    };

    enum class DebugType : std::uint8_t
    {
        DEFAULT,
        WIREFRAME,
        POINT
    };

    enum class ScriptState : uint8_t
    {
        UNINITIALIZED,
        LOADED,
        STARTED,
        FAILED
    };
}
