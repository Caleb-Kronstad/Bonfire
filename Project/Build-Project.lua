project "Project"
    location "../Build/Build-Files"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    files {
        "Source/**",
        "Assets/**"
    }

    includedirs {
        "Assets",
        -- Include Core
        "../Engine/Source",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.STB_IMAGE}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.JSON}"
    }

    links {
        "Engine",
        "GLAD",
        "GLFW",
        "IMGUI",
        "ASSIMP",
        "GLM",
        "IMGUIZMO"
    }

    targetdir ("%{wks.location}/../Build/Binaries/" .. OutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/../Build/Binaries-Intermediate/" .. OutputDir .. "/%{prj.name}")
    

    filter "system:windows"
        systemversion "latest"
        defines {
            "BONFIRE_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }
        postbuildcommands {
            "{COPY} %{wks.location}/../Project/Assets %{cfg.buildtarget.directory}/Assets", -- uncomment if building via build script "Windows-Build.bat"
            "{COPY} %{wks.location}/../Project/imgui.ini %{cfg.buildtarget.directory}/", -- uncomment if building via build script "Windows-Build.bat"
            -- "{COPY} %{wks.location}/../Project/Assets %{wks.location}/Build-Files/Assets", -- uncomment if building via IDE
            -- "{COPY} %{wks.location}/../Project/imgui.ini %{wks.location}/Build-Files/", -- uncomment if building via IDE
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
        postbuildcommands {
            "{COPY} %{wks.location}/../Project/Assets %{cfg.buildtarget.directory}/Assets",
            "{COPY} %{wks.location}/../Project/imgui.ini %{cfg.buildtarget.directory}/",
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
