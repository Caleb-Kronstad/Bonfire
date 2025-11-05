workspace "Bonfire"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Bonfire"
   location "Build"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

   -- Workspace-wide build options for GCC/Clang on Linux
   filter "system:linux"
      buildoptions { "-fPIC" }
      linkoptions { "-pthread" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

IncludeDir = {}
IncludeDir["GLAD"] = os.realpath("Engine/Dependencies/glad/include")
IncludeDir["GLFW"] = os.realpath("Engine/Dependencies/glfw/include")
IncludeDir["IMGUI"] = os.realpath("Engine/Dependencies/imgui")
IncludeDir["STB_IMAGE"] = os.realpath("Engine/Dependencies/stb_image")
IncludeDir["ASSIMP"] = os.realpath("Engine/Dependencies/assimp/include")
IncludeDir["GLM"] = os.realpath("Engine/Dependencies/glm/glm")
IncludeDir["IMGUIZMO"] = os.realpath("Engine/Dependencies/imguizmo")
IncludeDir["JSON"] = os.realpath("Engine/Dependencies/json")
IncludeDir["JOLT"] = os.realpath("Engine/Dependencies/jolt")
IncludeDir["ASIO"] = os.realpath("Engine/Dependencies/asio/include")
IncludeDir["LUA"] = os.realpath("Engine/Dependencies/lua-5.4.8/src")
IncludeDir["MINIAUDIO"] = os.realpath("Engine/Dependencies/miniaudio")

group "Dependencies"
   include "Engine/Dependencies/glad/Build-GLAD.lua"
   include "Engine/Dependencies/glfw/Build-GLFW.lua"
   include "Engine/Dependencies/imgui/Build-IMGUI.lua"
   include "Engine/Dependencies/assimp/Build-ASSIMP.lua"
   include "Engine/Dependencies/glm/Build-GLM.lua"
   include "Engine/Dependencies/imguizmo/Build-IMGUIZMO.lua"
   include "Engine/Dependencies/jolt/Build-JOLT.lua"
   include "Engine/Dependencies/lua-5.4.8/Build-LUA.lua"
group ""

group "Engine"
   include "Engine/Build-Engine.lua"
   include "Bonfire/Build-Bonfire.lua"
group ""