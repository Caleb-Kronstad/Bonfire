project "Engine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    staticruntime "on"

    pchheader "bonfire_pch.h"
    pchsource "Source/bonfire_pch.cpp"

    files {
        "Source/**.h",
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.c",

        "%{IncludeDir.STB_IMAGE}/**.h",
        "%{IncludeDir.STB_IMAGE}/**.cpp"
    }

    includedirs {
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.STB_IMAGE}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.IMGUIZMO}",

        "Source"
    }

    links {
        "GLAD",
        "GLFW",
        "IMGUI",
        "ASSIMP",
        "GLM",
        "IMGUIZMO",
        "opengl32.lib",
        "comdlg32.lib"
    }

    targetdir ("../bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("../bin-int/" .. OutputDir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"

        defines {
            "BONFIRE_PLATFORM_WINDOWS",
            "BONFIRE_BUILD_DLL",
            "_CRT_SECURE_NO_WARNINGS"
        }

        --postbuildcommands { -- build commands for dll
        --    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Sandbox")
        --}

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"