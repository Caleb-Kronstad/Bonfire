workspace "Bonfire"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Project"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

IncludeDir = {}
IncludeDir["GLAD"] = os.realpath("Engine/External/glad/include")
IncludeDir["GLFW"] = os.realpath("Engine/External/glfw/include")
IncludeDir["IMGUI"] = os.realpath("Engine/External/imgui")
IncludeDir["STB_IMAGE"] = os.realpath("Engine/External/stb_image")
IncludeDir["ASSIMP"] = os.realpath("Engine/External/assimp/include")
IncludeDir["GLM"] = os.realpath("Engine/External/glm/glm")
IncludeDir["IMGUIZMO"] = os.realpath("Engine/External/imguizmo")

group "_External"
   include "Engine/External/glad/Build-GLAD.lua"
   include "Engine/External/glfw/Build-GLFW.lua"
   include "Engine/External/imgui/Build-IMGUI.lua"
   include "Engine/External/assimp/Build-ASSIMP.lua"
   include "Engine/External/glm/Build-GLM.lua"
   include "Engine/External/imguizmo/Build-IMGUIZMO.lua"
group ""

group "Engine"
   include "Engine/Build-Engine.lua"
group ""

group "Projects"
   include "Project/Build-Project.lua"
group ""