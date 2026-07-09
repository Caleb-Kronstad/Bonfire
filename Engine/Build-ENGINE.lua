project "Engine"
    location "../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    defines {
        "GLM_ENABLE_EXPERIMENTAL"
    }

    files {
        "Source/**.h",
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.c",
        "%{IncludeDir.MINIAUDIO}/miniaudio.h}",
        "%{IncludeDir.MINIAUDIO}/miniaudio.c}",
        "%{IncludeDir.JSON}/**.hpp",
        "%{IncludeDir.ASIO}/**.hpp}"
    }

    includedirs {
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.SOIL}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.JSON}",
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.ASIO}",
        "%{IncludeDir.LUA}",
        "%{IncludeDir.MINIAUDIO}",
        "Source"
    }

    links {
        "GLAD",
        "GLFW",
        "IMGUI",
        "SOIL",
        "ASSIMP",
        "GLM",
        "IMGUIZMO",
        "JOLT",
        "LUA"
    }
    
    targetdir ("%{wks.location}/../Build/Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/../Build/Binaries-Intermediate/" .. OutputDir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        pchheader "bonfire_pch.hpp"
        pchsource "Source/bonfire_pch.cpp"
        defines {
            "DEBUG",
            "JPH_PROFILE_ENABLED",
            "JPH_DEBUG_RENDERER",
            "JPH_ENABLE_ASSERTS",
            "BONFIRE_PLATFORM_WINDOWS",
            "BONFIRE_BUILD_DLL",
            "_CRT_SECURE_NO_WARNINGS"
        }
        links {
            "opengl32",
            "comdlg32"
        }

    filter "system:linux"
        pchheader "Source/bonfire_pch.hpp"
        pchsource "Source/bonfire_pch.cpp"
        pic "On"
        defines {
            "BONFIRE_PLATFORM_LINUX",
            "JPH_PROFILE_ENABLED",
            "JPH_DEBUG_RENDERER"
        }
        links {
            "GL",
            "dl",
            "pthread"
        }

    filter "configurations:Debug"
        defines { "DEBUG", "JPH_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE", "JPH_ENABLE_ASSERTS" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
