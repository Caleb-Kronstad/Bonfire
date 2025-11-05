#include "bonfire_pch.hpp"
#include "LuaScript.hpp"

#include "LuaBindings.hpp"

namespace Bonfire
{
    LuaScript::LuaScript(uint32_t param_id, const std::string& name, const std::string& path)
        :param_id(param_id), name(name), path(path)
    {
    }
    LuaScript::~LuaScript()
    {
        if (lua_state && script_ref != LUA_NOREF)
        {
            luaL_unref(lua_state, LUA_REGISTRYINDEX, script_ref);
            script_ref = LUA_NOREF;
        }
    }

    bool LuaScript::Load(lua_State* lua_state)
    {
        this->lua_state = lua_state;
        if (luaL_dofile(lua_state, path.c_str()) != LUA_OK)
        {
            Log::Error("Failed to load Lua script: " + path);
            Log::Error(lua_tostring(lua_state, -1));
            lua_pop(lua_state, 1);
            state = ScriptState::FAILED;
            return false;
        }

        if (!lua_istable(lua_state, -1))
        {
            Log::Error("Script " + name + " must return a table");
            lua_pop(lua_state, 1);
            state = ScriptState::FAILED;
            return false;
        }

        script_ref = luaL_ref(lua_state, LUA_REGISTRYINDEX);
        if (!ValidateScript(lua_state))
        {
            state = ScriptState::FAILED;
            return false;
        }

        state = ScriptState::LOADED;
        Log::Info("Loaded Lua script: " + name);
        return true;
    }

    bool LuaScript::ValidateScript(lua_State* lua_state)
    {
        lua_rawgeti(lua_state, LUA_REGISTRYINDEX, script_ref);

        bool has_on_start = false;
        bool has_on_update = false;
        bool has_on_destroy = false;

        lua_getfield(lua_state, -1, "OnStart");
        has_on_start = lua_isfunction(lua_state, -1);
        lua_pop(lua_state, 1);

        lua_getfield(lua_state, -1, "OnUpdate");
        has_on_update = lua_isfunction(lua_state, -1);
        lua_pop(lua_state, 1);

        lua_getfield(lua_state, -1, "OnDestroy");
        has_on_destroy = lua_isfunction(lua_state, -1);
        lua_pop(lua_state, 1);

        lua_pop(lua_state, 1);

        if (!has_on_start && !has_on_update && !has_on_destroy)
        {
            Log::Warning("Script " + name + " has no lifecycle functions");
        }

        return true;
    }

    bool LuaScript::CallOnStart(Entity* entity)
    {
        if (state != ScriptState::LOADED && state != ScriptState::STARTED)
            return false;

        bool result = CallLuaFunction("OnStart", entity);
        if (result)
            state = ScriptState::STARTED;

        return result;
    }

    bool LuaScript::CallOnUpdate(Entity* entity, float delta_time)
    {
        if (state != ScriptState::STARTED || !enabled)
            return false;

        return CallLuaFunction("OnUpdate", entity, delta_time);
    }

    bool LuaScript::CallOnDestroy(Entity* entity)
    {
        if (state != ScriptState::STARTED)
            return false;

        return CallLuaFunction("OnDestroy", entity);
    }

    bool LuaScript::CallLuaFunction(const std::string& function_name, Entity* entity, float delta_time)
    {
        if (!lua_state || script_ref == LUA_NOREF)
            return false;

        lua_rawgeti(lua_state, LUA_REGISTRYINDEX, script_ref);
        lua_getfield(lua_state, -1, function_name.c_str());

        if (!lua_isfunction(lua_state, -1))
        {
            lua_pop(lua_state, 2);
            return true;
        }

        lua_pushvalue(lua_state, -2);
        LuaBindings::PushEntity(lua_state, entity);

        int num_args = 2;
        if (function_name == "OnUpdate")
        {
            lua_pushnumber(lua_state, delta_time);
            num_args = 3;
        }

        if (lua_pcall(lua_state, num_args, 0, 0) != LUA_OK)
        {
            HandleError(lua_state, function_name);
            lua_pop(lua_state, 1);
            return false;
        }

        lua_pop(lua_state, 1);
        return true;
    }

    void LuaScript::HandleError(lua_State* lua_state, const std::string& function_name)
    {
        const char* error_msg = lua_tostring(lua_state, -1);
        Log::Error("Error in " + name + "::" + function_name + "(): " + (error_msg ? error_msg : "unknown error"));
    }

}