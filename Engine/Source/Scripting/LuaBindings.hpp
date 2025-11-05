#pragma once

#include "Renderer/Entity.hpp"

namespace Bonfire
{
    class LuaBindings
    {
    public:
        static void RegisterEntityBindings(lua_State* L);
        static void RegisterVec3Bindings(lua_State* L);
        static void RegisterInputBindings(lua_State* L);
        static void RegisterCameraBindings(lua_State* L);
        static void RegisterSceneBindings(lua_State* L);
        static void RegisterProjectBindings(lua_State* L);

        static void PushEntity(lua_State* L, Entity* entity);
        static Entity* CheckEntity(lua_State* L, int index);
        static void PushVec3(lua_State* L, const glm::vec3& vec);
        static glm::vec3 CheckVec3(lua_State* L, int index);
        
    public:
        static const char* ENTITY_METATABLE;
        static const char* VEC3_METATABLE;
    };
}
