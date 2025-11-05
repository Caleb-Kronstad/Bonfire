#pragma once

#include "BonfireEnums.hpp"

namespace Bonfire
{
    class Entity;
    
    class LuaScript
    {
    public:
        LuaScript(uint32_t param_id, const std::string& name, const std::string& path);
        ~LuaScript();

        bool Load(lua_State* lua_state);
        bool CallOnStart(Entity* entity);
        bool CallOnUpdate(Entity* entity, float delta_time);
        bool CallOnDestroy(Entity* entity);

        ScriptState GetState() const { return state; }

    private:
        bool ValidateScript(lua_State* lua_state);
        void HandleError(lua_State* lua_state, const std::string& function_name);
        bool CallLuaFunction(const std::string& function_name, Entity* entity, float delta_time = 0.0f);

    public:
        uint32_t param_id;
        std::string name;
        std::string path;
        bool enabled = true;

    private:
        ScriptState state = ScriptState::UNINITIALIZED;
        int script_ref = LUA_NOREF;
        lua_State* lua_state = nullptr;
    };
}
