project "Engine"
    location "../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

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
        "IMGUIZMO"
    }
    
    targetdir ("%{wks.location}/../Build/Binaries/" .. OutputDir .. "/Project/%{prj.name}")
    objdir ("%{wks.location}/../Build/Binaries-Intermediate/" .. OutputDir .. "/Project/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        pchheader "bonfire_pch.hpp"
        pchsource "Source/bonfire_pch.cpp"
        defines {
            "BONFIRE_PLATFORM_WINDOWS",
            "BONFIRE_BUILD_DLL",
            "_CRT_SECURE_NO_WARNINGS"
        }
        links {
            "opengl32",
            "comdlg32"
        }
        --postbuildcommands {
        --    ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. OutputDir .. "/Project")
        --}

    filter "system:linux"
        pchheader "Source/bonfire_pch.hpp"
        pchsource "Source/bonfire_pch.cpp"
        pic "On"
        defines {
            "BONFIRE_PLATFORM_LINUX"
        }
        links {
            "GL",
            "dl",
            "pthread"
        }

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
