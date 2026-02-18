#include "bonfire_pch.hpp"
#include "LuaBindings.hpp"

#include "Renderer/Entity.hpp"
#include "Input/InputCodes.hpp"

#include "Core/Engine.hpp"

namespace Bonfire
{
    const char* LuaBindings::ENTITY_METATABLE = "Bonfire.Entity";
    const char* LuaBindings::VEC3_METATABLE = "Bonfire.Vec3";

    static int lua_vec3_new(lua_State* lua_state)
    {
        float x = static_cast<float>(luaL_optnumber(lua_state, 1, 0.0));
        float y = static_cast<float>(luaL_optnumber(lua_state, 2, 0.0));
        float z = static_cast<float>(luaL_optnumber(lua_state, 3, 0.0));

        LuaBindings::PushVec3(lua_state, glm::vec3(x, y, z));
        return 1;
    }

    static int lua_vec3_index(lua_State* lua_state)
    {
        glm::vec3* vec = static_cast<glm::vec3*>(luaL_checkudata(lua_state, 1, LuaBindings::VEC3_METATABLE));
        const char* key = luaL_checkstring(lua_state, 2);

        if (strcmp(key, "x") == 0)
            lua_pushnumber(lua_state, vec->x);
        else if (strcmp(key, "y") == 0)
            lua_pushnumber(lua_state, vec->y);
        else if (strcmp(key, "z") == 0)
            lua_pushnumber(lua_state, vec->z);
        else
            lua_pushnil(lua_state);

        return 1;
    }

    static int lua_vec3_newindex(lua_State* lua_state)
    {
        glm::vec3* vec = static_cast<glm::vec3*>(luaL_checkudata(lua_state, 1, LuaBindings::VEC3_METATABLE));
        const char* key = luaL_checkstring(lua_state, 2);
        float value = static_cast<float>(luaL_checknumber(lua_state, 3));

        if (strcmp(key, "x") == 0)
            vec->x = value;
        else if (strcmp(key, "y") == 0)
            vec->y = value;
        else if (strcmp(key, "z") == 0)
            vec->z = value;

        return 0;
    }

    static int lua_vec3_add(lua_State* lua_state)
    {
        glm::vec3 a = LuaBindings::CheckVec3(lua_state, 1);
        glm::vec3 b = LuaBindings::CheckVec3(lua_state, 2);
        LuaBindings::PushVec3(lua_state, a + b);
        return 1;
    }

    static int lua_vec3_sub(lua_State* lua_state)
    {
        glm::vec3 a = LuaBindings::CheckVec3(lua_state, 1);
        glm::vec3 b = LuaBindings::CheckVec3(lua_state, 2);
        LuaBindings::PushVec3(lua_state, a - b);
        return 1;
    }

    static int lua_vec3_mul(lua_State* lua_state)
    {
        glm::vec3 vec = LuaBindings::CheckVec3(lua_state, 1);
        float scalar = static_cast<float>(luaL_checknumber(lua_state, 2));
        LuaBindings::PushVec3(lua_state, vec * scalar);
        return 1;
    }

    static int lua_vec3_tostring(lua_State* lua_state)
    {
        glm::vec3 vec = LuaBindings::CheckVec3(lua_state, 1);
        lua_pushfstring(lua_state, "Vec3(%f, %f, %f)", vec.x, vec.y, vec.z);
        return 1;
    }

    void LuaBindings::RegisterVec3Bindings(lua_State* lua_state)
    {
        luaL_newmetatable(lua_state, VEC3_METATABLE);

        lua_pushstring(lua_state, "__index");
        lua_pushcfunction(lua_state, lua_vec3_index);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "__newindex");
        lua_pushcfunction(lua_state, lua_vec3_newindex);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "__add");
        lua_pushcfunction(lua_state, lua_vec3_add);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "__sub");
        lua_pushcfunction(lua_state, lua_vec3_sub);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "__mul");
        lua_pushcfunction(lua_state, lua_vec3_mul);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "__tostring");
        lua_pushcfunction(lua_state, lua_vec3_tostring);
        lua_settable(lua_state, -3);

        lua_pop(lua_state, 1);

        lua_register(lua_state, "Vec3", lua_vec3_new);
    }
    
    void LuaBindings::RegisterSceneBindings(lua_State* lua_state)
    {
        lua_register(lua_state, "LoadScene", [](lua_State* L) -> int {
          const char* scene_path = luaL_checkstring(L, 1);
          Engine::GetRenderer().GetScene().LoadScene(Engine::GetRenderer().GetParamDatabase());
          return 0;
        });

        lua_register(lua_state, "NextScene", [](lua_State* L) -> int
        {
            unsigned int scene_index = luaL_checkinteger(L, 1);
            Engine::GetRenderer().LoadScene(scene_index);
            return 0;
        });
    }

    void LuaBindings::RegisterProjectBindings(lua_State* lua_state)
    {
        lua_register(lua_state, "QuitProject", [](lua_State* L) -> int {
            Engine::Instance().SetProjectRunState(false);
            return 0;
        });

        lua_register(lua_state, "GetProjectName", [](lua_State* L) -> int {
            lua_pushstring(L, Engine::Instance().GetProjectConfig().project_name.c_str());
            return 1;
        });

        lua_register(lua_state, "GetDeltaTime", [](lua_State* L) -> int {
            lua_pushnumber(L, Engine::Instance().GetDeltaTime());
            return 1;
        });
    }

    // ENTITY BINDINGS
    static int lua_entity_get_position(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        LuaBindings::PushVec3(lua_state, entity->position);
        return 1;
    }

    static int lua_entity_set_position(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        glm::vec3 pos = LuaBindings::CheckVec3(lua_state, 2);
        entity->position = pos;
        return 0;
    }

    static int lua_entity_get_rotation(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        LuaBindings::PushVec3(lua_state, entity->rotation);
        return 1;
    }

    static int lua_entity_set_rotation(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        glm::vec3 rot = LuaBindings::CheckVec3(lua_state, 2);
        entity->rotation = rot;
        return 0;
    }

    static int lua_entity_get_scale(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        LuaBindings::PushVec3(lua_state, entity->scale);
        return 1;
    }

    static int lua_entity_set_scale(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        glm::vec3 scale = LuaBindings::CheckVec3(lua_state, 2);
        entity->scale = scale;
        return 0;
    }

    static int lua_entity_get_name(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        lua_pushstring(lua_state, entity->name.c_str());
        return 1;
    }

    static int lua_entity_set_name(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        const char* name = luaL_checkstring(lua_state, 2);
        entity->name = name;
        return 0;
    }

    static int lua_entity_get_enabled(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        lua_pushboolean(lua_state, entity->enabled);
        return 1;
    }

    static int lua_entity_set_enabled(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        bool enabled = lua_toboolean(lua_state, 2);
        entity->enabled = enabled;
        return 0;
    }

    static int lua_entity_get_forward(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        LuaBindings::PushVec3(lua_state, entity->GetForwardVector());
        return 1;
    }

    static int lua_entity_get_right(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        LuaBindings::PushVec3(lua_state, entity->GetRightVector());
        return 1;
    }

    static int lua_entity_get_up(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        LuaBindings::PushVec3(lua_state, entity->GetUpVector());
        return 1;
    }
    
    // PHYSICS COMPONENT BINDINGS
    static int lua_entity_has_physics(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        bool has_physics = entity->HasComponent<PhysicsComponent>();
        lua_pushboolean(lua_state, has_physics);
        return 1;
    }
    
    static int lua_entity_get_physics_velocity(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
  
        if (!entity->HasComponent<PhysicsComponent>())
        {
            lua_pushnil(lua_state);
            return 1;
        }
  
        PhysicsComponent& physics_comp = entity->GetComponent<PhysicsComponent>();
        if (!physics_comp.physics_body)
        {
            lua_pushnil(lua_state);
            return 1;
        }
  
        glm::vec3 velocity = physics_comp.physics_body->GetLinearVelocity();
        LuaBindings::PushVec3(lua_state, velocity);
        return 1;
    }
  
    static int lua_entity_set_physics_velocity(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        glm::vec3 velocity = LuaBindings::CheckVec3(lua_state, 2);
  
        if (!entity->HasComponent<PhysicsComponent>())
            return 0;
  
        PhysicsComponent& physics_comp = entity->GetComponent<PhysicsComponent>();
        if (!physics_comp.physics_body)
            return 0;
  
        physics_comp.physics_body->SetLinearVelocity(velocity);
        return 0;
    }
    
    static int lua_entity_add_physics_impulse(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        glm::vec3 impulse = LuaBindings::CheckVec3(lua_state, 2);
  
        if (!entity->HasComponent<PhysicsComponent>())
            return 0;
  
        PhysicsComponent& physics_comp = entity->GetComponent<PhysicsComponent>();
        if (!physics_comp.physics_body)
            return 0;
  
        physics_comp.physics_body->AddImpulse(impulse);
        return 0;
    }
  
    static int lua_entity_set_physics_gravity(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        float gravity_factor = static_cast<float>(luaL_checknumber(lua_state, 2));
  
        if (!entity->HasComponent<PhysicsComponent>())
            return 0;
  
        PhysicsComponent& physics_comp = entity->GetComponent<PhysicsComponent>();
        if (!physics_comp.physics_body)
            return 0;
  
        physics_comp.physics_body->SetGravityFactor(gravity_factor);
        return 0;
    }

    static int lua_entity_set_physics_rotation_yaw(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        float yaw_degrees = static_cast<float>(luaL_checknumber(lua_state, 2));

        if (!entity->HasComponent<PhysicsComponent>())
            return 0;

        PhysicsComponent& physics_comp = entity->GetComponent<PhysicsComponent>();
        if (!physics_comp.physics_body)
            return 0;

        glm::quat current_quat = physics_comp.physics_body->GetRotation();
        glm::vec3 current_euler = glm::degrees(glm::eulerAngles(current_quat));
        glm::vec3 new_euler = glm::vec3(current_euler.x, yaw_degrees, current_euler.z);

        physics_comp.physics_body->SetRotation(glm::quat(glm::radians(new_euler)));
        return 0;
    }
    
    // CAMERA COMPONENT BINDINGS
    static int lua_entity_has_camera(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        bool has_camera = entity->HasComponent<CameraComponent>();
        lua_pushboolean(lua_state, has_camera);
        return 1;
    }
    
    static int lua_entity_get_camera_position(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
  
        if (!entity->HasComponent<CameraComponent>())
        {
            lua_pushnil(lua_state);
            return 1;
        }
  
        CameraComponent& camera_comp = entity->GetComponent<CameraComponent>();
        if (!camera_comp.camera)
        {
            lua_pushnil(lua_state);
            return 1;
        }
  
        LuaBindings::PushVec3(lua_state, camera_comp.camera->position);
        return 1;
    }
  
    static int lua_entity_set_camera_position(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        glm::vec3 position = LuaBindings::CheckVec3(lua_state, 2);
  
        if (!entity->HasComponent<CameraComponent>())
            return 0;
  
        CameraComponent& camera_comp = entity->GetComponent<CameraComponent>();
        if (!camera_comp.camera)
            return 0;
  
        camera_comp.camera->position = position;
        return 0;
    }
    
    static int lua_entity_get_camera_yaw(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
  
        if (!entity->HasComponent<CameraComponent>())
        {
            lua_pushnumber(lua_state, 0.0f);
            return 1;
        }
  
        CameraComponent& camera_comp = entity->GetComponent<CameraComponent>();
        if (!camera_comp.camera)
        {
            lua_pushnumber(lua_state, 0.0f);
            return 1;
        }
  
        lua_pushnumber(lua_state, camera_comp.camera->yaw);
        return 1;
    }
  
    static int lua_entity_set_camera_yaw(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        float yaw = static_cast<float>(luaL_checknumber(lua_state, 2));
  
        if (!entity->HasComponent<CameraComponent>())
            return 0;
  
        CameraComponent& camera_comp = entity->GetComponent<CameraComponent>();
        if (!camera_comp.camera)
            return 0;
  
        camera_comp.camera->yaw = yaw;
        camera_comp.camera->UpdateCameraVectors();
        return 0;
    }
    
    static int lua_entity_get_camera_pitch(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
  
        if (!entity->HasComponent<CameraComponent>())
        {
            lua_pushnumber(lua_state, 0.0f);
            return 1;
        }
  
        CameraComponent& camera_comp = entity->GetComponent<CameraComponent>();
        if (!camera_comp.camera)
        {
            lua_pushnumber(lua_state, 0.0f);
            return 1;
        }
  
        lua_pushnumber(lua_state, camera_comp.camera->pitch);
        return 1;
    }
  
    static int lua_entity_set_camera_pitch(lua_State* lua_state)
    {
        Entity* entity = LuaBindings::CheckEntity(lua_state, 1);
        float pitch = static_cast<float>(luaL_checknumber(lua_state, 2));
  
        if (!entity->HasComponent<CameraComponent>())
            return 0;
  
        CameraComponent& camera_comp = entity->GetComponent<CameraComponent>();
        if (!camera_comp.camera)
            return 0;
  
        camera_comp.camera->pitch = pitch;
        camera_comp.camera->UpdateCameraVectors();
        return 0;
    }

    void LuaBindings::RegisterEntityBindings(lua_State* lua_state)
    {
        luaL_newmetatable(lua_state, ENTITY_METATABLE);

        lua_pushstring(lua_state, "__index");
        lua_newtable(lua_state);

        lua_pushstring(lua_state, "GetPosition");
        lua_pushcfunction(lua_state, lua_entity_get_position);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "SetPosition");
        lua_pushcfunction(lua_state, lua_entity_set_position);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetRotation");
        lua_pushcfunction(lua_state, lua_entity_get_rotation);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "SetRotation");
        lua_pushcfunction(lua_state, lua_entity_set_rotation);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetScale");
        lua_pushcfunction(lua_state, lua_entity_get_scale);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "SetScale");
        lua_pushcfunction(lua_state, lua_entity_set_scale);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetName");
        lua_pushcfunction(lua_state, lua_entity_get_name);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "SetName");
        lua_pushcfunction(lua_state, lua_entity_set_name);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetEnabled");
        lua_pushcfunction(lua_state, lua_entity_get_enabled);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "SetEnabled");
        lua_pushcfunction(lua_state, lua_entity_set_enabled);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetForward");
        lua_pushcfunction(lua_state, lua_entity_get_forward);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetRight");
        lua_pushcfunction(lua_state, lua_entity_get_right);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetUp");
        lua_pushcfunction(lua_state, lua_entity_get_up);
        lua_settable(lua_state, -3);
        
        lua_pushstring(lua_state, "HasPhysics");
        lua_pushcfunction(lua_state, lua_entity_has_physics);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "GetPhysicsVelocity");
        lua_pushcfunction(lua_state, lua_entity_get_physics_velocity);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "SetPhysicsVelocity");
        lua_pushcfunction(lua_state, lua_entity_set_physics_velocity);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "AddPhysicsImpulse");
        lua_pushcfunction(lua_state, lua_entity_add_physics_impulse);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "SetPhysicsGravity");
        lua_pushcfunction(lua_state, lua_entity_set_physics_gravity);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "SetPhysicsRotationYaw");
        lua_pushcfunction(lua_state, lua_entity_set_physics_rotation_yaw);
        lua_settable(lua_state, -3);
        
        lua_pushstring(lua_state, "HasCamera");
        lua_pushcfunction(lua_state, lua_entity_has_camera);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "GetCameraPosition");
        lua_pushcfunction(lua_state, lua_entity_get_camera_position);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "SetCameraPosition");
        lua_pushcfunction(lua_state, lua_entity_set_camera_position);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "GetCameraYaw");
        lua_pushcfunction(lua_state, lua_entity_get_camera_yaw);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "SetCameraYaw");
        lua_pushcfunction(lua_state, lua_entity_set_camera_yaw);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "GetCameraPitch");
        lua_pushcfunction(lua_state, lua_entity_get_camera_pitch);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "SetCameraPitch");
        lua_pushcfunction(lua_state, lua_entity_set_camera_pitch);
        lua_settable(lua_state, -3);

        lua_settable(lua_state, -3);
        lua_pop(lua_state, 1);
    }

    // INPUT BINDINGS
    static int lua_input_is_key_pressed(lua_State* lua_state)
    {
        int key = luaL_checkinteger(lua_state, 1);
    
        Engine& project = Engine::Instance();
        Window& window = project.GetWindow();
        GLFWwindow* glfw_window = window.GetNativeWindow();
    
        int state = glfwGetKey(glfw_window, key);
        lua_pushboolean(lua_state, state == GLFW_PRESS);
        return 1;
    }
    
    static int lua_input_is_mouse_button_pressed(lua_State* lua_state)
    {
        int button = luaL_checkinteger(lua_state, 1);
    
        Engine& project = Engine::Instance();
        Window& window = project.GetWindow();
        GLFWwindow* glfw_window = window.GetNativeWindow();
    
        int state = glfwGetMouseButton(glfw_window, button);
        lua_pushboolean(lua_state, state == GLFW_PRESS);
        return 1;
    }
    
    static int lua_input_get_mouse_position(lua_State* lua_state)
    {
        Engine& project = Engine::Instance();
        Window& window = project.GetWindow();
        GLFWwindow* glfw_window = window.GetNativeWindow();
    
        double xpos, ypos;
        glfwGetCursorPos(glfw_window, &xpos, &ypos);
    
        lua_pushnumber(lua_state, xpos);
        lua_pushnumber(lua_state, ypos);
        return 2;
    }
    
    static int lua_input_get_mouse_x(lua_State* lua_state)
    {
        Engine& project = Engine::Instance();
        Window& window = project.GetWindow();
        GLFWwindow* glfw_window = window.GetNativeWindow();
    
        double xpos, ypos;
        glfwGetCursorPos(glfw_window, &xpos, &ypos);
    
        lua_pushnumber(lua_state, xpos);
        return 1;
    }
    
    static int lua_input_get_mouse_y(lua_State* lua_state)
    {
       Engine& project = Engine::Instance();
       Window& window = project.GetWindow();
       GLFWwindow* glfw_window = window.GetNativeWindow();
    
       double xpos, ypos;
       glfwGetCursorPos(glfw_window, &xpos, &ypos);
    
       lua_pushnumber(lua_state, ypos);
       return 1;
    }
    
    static int lua_input_set_cursor_mode(lua_State* lua_state)
    {
        int mode = luaL_checkinteger(lua_state, 1);
  
        Engine& project = Engine::Instance();
        Window& window = project.GetWindow();
        GLFWwindow* glfw_window = window.GetNativeWindow();
  
        if (glfw_window)
        {
            glfwSetInputMode(glfw_window, GLFW_CURSOR, mode);
        }
  
        return 0;
    }

    void LuaBindings::RegisterInputBindings(lua_State* lua_state)
    {
        lua_newtable(lua_state);
  
        // Input query functions
        lua_pushstring(lua_state, "IsKeyPressed");
        lua_pushcfunction(lua_state, lua_input_is_key_pressed);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "IsMouseButtonPressed");
        lua_pushcfunction(lua_state, lua_input_is_mouse_button_pressed);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "GetMousePosition");
        lua_pushcfunction(lua_state, lua_input_get_mouse_position);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "GetMouseX");
        lua_pushcfunction(lua_state, lua_input_get_mouse_x);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "GetMouseY");
        lua_pushcfunction(lua_state, lua_input_get_mouse_y);
        lua_settable(lua_state, -3);
  
        // Printable keys
        lua_pushstring(lua_state, "KEY_SPACE");
        lua_pushinteger(lua_state, InputCode::Space);
        lua_settable(lua_state, -3);
  
        // Number keys
        lua_pushstring(lua_state, "KEY_0");
        lua_pushinteger(lua_state, InputCode::D0);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_1");
        lua_pushinteger(lua_state, InputCode::D1);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_2");
        lua_pushinteger(lua_state, InputCode::D2);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_3");
        lua_pushinteger(lua_state, InputCode::D3);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_4");
        lua_pushinteger(lua_state, InputCode::D4);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_5");
        lua_pushinteger(lua_state, InputCode::D5);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_6");
        lua_pushinteger(lua_state, InputCode::D6);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_7");
        lua_pushinteger(lua_state, InputCode::D7);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_8");
        lua_pushinteger(lua_state, InputCode::D8);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_9");
        lua_pushinteger(lua_state, InputCode::D9);
        lua_settable(lua_state, -3);
  
        // Letter keys
       lua_pushstring(lua_state, "KEY_A");
        lua_pushinteger(lua_state, InputCode::A);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_B");
        lua_pushinteger(lua_state, InputCode::B);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_C");
        lua_pushinteger(lua_state, InputCode::C);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_D");
        lua_pushinteger(lua_state, InputCode::D);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_E");
        lua_pushinteger(lua_state, InputCode::E);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_F");
        lua_pushinteger(lua_state, InputCode::F);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_G");
        lua_pushinteger(lua_state, InputCode::G);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_H");
        lua_pushinteger(lua_state, InputCode::H);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_I");
        lua_pushinteger(lua_state, InputCode::I);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_J");
        lua_pushinteger(lua_state, InputCode::J);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "KEY_K");
        lua_pushinteger(lua_state, InputCode::K);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_L");
        lua_pushinteger(lua_state, InputCode::L);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_M");
        lua_pushinteger(lua_state, InputCode::M);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_N");
        lua_pushinteger(lua_state, InputCode::N);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_O");
        lua_pushinteger(lua_state,InputCode::O);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_P");
        lua_pushinteger(lua_state, InputCode::P);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_Q");
        lua_pushinteger(lua_state, InputCode::Q);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_R");
        lua_pushinteger(lua_state, InputCode::R);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_S");
        lua_pushinteger(lua_state, InputCode::S);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_T");
        lua_pushinteger(lua_state, InputCode::T);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_U");
        lua_pushinteger(lua_state, InputCode::U);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_V");
        lua_pushinteger(lua_state, InputCode::V);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_W");
        lua_pushinteger(lua_state, InputCode::W);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_X");
        lua_pushinteger(lua_state, InputCode::X);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_Y");
        lua_pushinteger(lua_state, InputCode::Y);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_Z");
        lua_pushinteger(lua_state, InputCode::Z);
        lua_settable(lua_state, -3);
  
        // Function keys
        lua_pushstring(lua_state, "KEY_ESCAPE");
        lua_pushinteger(lua_state, InputCode::Escape);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_ENTER");
        lua_pushinteger(lua_state, InputCode::Enter);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_TAB");
        lua_pushinteger(lua_state, InputCode::Tab);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_BACKSPACE");
        lua_pushinteger(lua_state, InputCode::Backspace);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_INSERT");
        lua_pushinteger(lua_state, InputCode::Insert);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_DELETE");
        lua_pushinteger(lua_state, InputCode::Delete);
        lua_settable(lua_state, -3);
 
        // Arrow keys
        lua_pushstring(lua_state, "KEY_RIGHT");
        lua_pushinteger(lua_state, InputCode::Right);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_LEFT");
        lua_pushinteger(lua_state, InputCode::Left);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_DOWN");
        lua_pushinteger(lua_state, InputCode::Down);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_UP");
        lua_pushinteger(lua_state, InputCode::Up);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_PAGE_UP");
        lua_pushinteger(lua_state, InputCode::PageUp);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_PAGE_DOWN");
        lua_pushinteger(lua_state, InputCode::PageDown);
        lua_settable(lua_state, -3);
 
        lua_pushstring(lua_state, "KEY_HOME");
        lua_pushinteger(lua_state, InputCode::Home);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "KEY_END");
        lua_pushinteger(lua_state, InputCode::End);
        lua_settable(lua_state, -3);

        // F keys
        lua_pushstring(lua_state, "KEY_F1");
        lua_pushinteger(lua_state, InputCode::F1);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F2");
        lua_pushinteger(lua_state, InputCode::F2);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F3");
        lua_pushinteger(lua_state, InputCode::F3);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F4");
        lua_pushinteger(lua_state, InputCode::F4);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F5");
        lua_pushinteger(lua_state, InputCode::F5);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F6");
        lua_pushinteger(lua_state, InputCode::F6);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F7");
        lua_pushinteger(lua_state, InputCode::F7);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F8");
        lua_pushinteger(lua_state, InputCode::F8);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F9");
        lua_pushinteger(lua_state, InputCode::F9);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F10");
        lua_pushinteger(lua_state, InputCode::F10);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "KEY_F11");
        lua_pushinteger(lua_state, InputCode::F11);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_F12");
        lua_pushinteger(lua_state, InputCode::F12);
        lua_settable(lua_state, -3);
  
        // Modifier keys
        lua_pushstring(lua_state, "KEY_LEFT_SHIFT");
        lua_pushinteger(lua_state, InputCode::LeftShift);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_LEFT_CONTROL");
        lua_pushinteger(lua_state, InputCode::LeftControl);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_LEFT_ALT");
        lua_pushinteger(lua_state, InputCode::LeftAlt);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_RIGHT_SHIFT");
        lua_pushinteger(lua_state, InputCode::RightShift);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_RIGHT_CONTROL");
        lua_pushinteger(lua_state, InputCode::RightControl);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "KEY_RIGHT_ALT");
        lua_pushinteger(lua_state, InputCode::RightAlt);
        lua_settable(lua_state, -3);
  
        // Mouse buttons
        lua_pushstring(lua_state, "MOUSE_BUTTON_LEFT");
        lua_pushinteger(lua_state, InputCode::Button0);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "MOUSE_BUTTON_RIGHT");
        lua_pushinteger(lua_state, InputCode::Button1);
        lua_settable(lua_state, -3);
  
        lua_pushstring(lua_state, "MOUSE_BUTTON_MIDDLE");
        lua_pushinteger(lua_state, InputCode::Button2);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "MOUSE_BUTTON_4");
        lua_pushinteger(lua_state, InputCode::Button3);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "MOUSE_BUTTON_5");
        lua_pushinteger(lua_state, InputCode::Button4);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "MOUSE_BUTTON_6");
        lua_pushinteger(lua_state, InputCode::Button5);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "MOUSE_BUTTON_7");
        lua_pushinteger(lua_state, InputCode::Button6);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "MOUSE_BUTTON_8");
        lua_pushinteger(lua_state, InputCode::Button7);
        lua_settable(lua_state, -3);
        
        // Cursor control function
        lua_pushstring(lua_state, "SetCursorMode");
        lua_pushcfunction(lua_state, lua_input_set_cursor_mode);
        lua_settable(lua_state, -3);
      
        // Cursor mode constants
        lua_pushstring(lua_state, "CURSOR_NORMAL");
        lua_pushinteger(lua_state, GLFW_CURSOR_NORMAL);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "CURSOR_HIDDEN");
        lua_pushinteger(lua_state, GLFW_CURSOR_HIDDEN);
        lua_settable(lua_state, -3);
      
        lua_pushstring(lua_state, "CURSOR_DISABLED");
        lua_pushinteger(lua_state, GLFW_CURSOR_DISABLED);
        lua_settable(lua_state, -3);
  
        lua_setglobal(lua_state, "Input");
    }

    // HELPER FUNCTIONS
    void LuaBindings::PushEntity(lua_State* lua_state, Entity* entity)
    {
        Entity** userdata = static_cast<Entity**>(lua_newuserdata(lua_state, sizeof(Entity*)));
        *userdata = entity;
        luaL_setmetatable(lua_state, ENTITY_METATABLE);
    }

    Entity* LuaBindings::CheckEntity(lua_State* lua_state, int index)
    {
        Entity** userdata = static_cast<Entity**>(luaL_checkudata(lua_state, index, ENTITY_METATABLE));
        return *userdata;
    }

    void LuaBindings::PushVec3(lua_State* lua_state, const glm::vec3& vec)
    {
        glm::vec3* userdata = static_cast<glm::vec3*>(lua_newuserdata(lua_state, sizeof(glm::vec3)));
        *userdata = vec;
        luaL_setmetatable(lua_state, VEC3_METATABLE);
    }

    glm::vec3 LuaBindings::CheckVec3(lua_State* lua_state, int index)
    {
        glm::vec3* userdata = static_cast<glm::vec3*>(luaL_checkudata(lua_state, index, VEC3_METATABLE));
        return *userdata;
    }
}