project "Editor"
    location "../Build/Build-Files"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    files {
        "Source/**",
        "Data/**",
        "../Editor",
    }

    includedirs {
        "Source",
        "Data",
        -- Include Core
        "../Engine/Source",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.STB_IMAGE}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.JSON}",
        "%{IncludeDir.JOLT}",
        "%{IncludeDir.LUA}",
        "%{IncludeDir.MINIAUDIO}"
    }

    links {
        "Engine",
        "GLAD",
        "GLFW",
        "IMGUI",
        "ASSIMP",
        "GLM",
        "IMGUIZMO",
        "JOLT",
        "LUA"
    }

    targetdir ("%{wks.location}/../Build/Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/../Build/Binaries-Intermediate/" .. OutputDir .. "/%{prj.name}")
    
    postbuildcommands {
        --"{COPY} %{wks.location}/../Editor/Data %{cfg.buildtarget.directory}/Data",
        --"{COPY} %{wks.location}/../Editor/imgui.ini %{cfg.buildtarget.directory}/",
    }

    defines {
        "JPH_DEBUG_RENDERER",
        "JPH_PROFILE_ENABLED"
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "BONFIRE_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        defines {
            "BONFIRE_PLATFORM_LINUX"
        }
        links {
            "GL",
            "dl",
            "pthread",
            "X11",
            "Xrandr",
            "Xi",
            "Xxf86vm",
            "Xcursor",
            "Xinerama"
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
