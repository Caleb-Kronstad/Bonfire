project "Project"
    location "../Build/Build-Files"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    files {
        "Source/**.h",
        "Source/**.cpp",
        "Source/**.hpp",
        "Source/**.c",
        "Resources/**.vert",
        "Resources/**.frag",
        "Resources/**.geom",
        "Resources/**.obj",
        "Resources/**.mtl",
        "Resources/**.blend",
        "Resources/**.blend1",
        "Resources/**.png",
        "Resources/**.jpg",
        "Resources/**.ttf"
    }

    includedirs {
        "Source",
        "Resources",
        -- Include Core
        "../Engine/Source",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.STB_IMAGE}",
        "%{IncludeDir.ASSIMP}",
        "%{IncludeDir.GLM}",
        "%{IncludeDir.IMGUIZMO}"
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
            -- "{COPY} %{wks.location}/../Project/Resources %{cfg.buildtarget.directory}/Resources", -- UNCOMMENT THIS LINE IF USING Windows-Build.bat TO BUILD INSTEAD OF AN IDE
            -- "{COPY} %{wks.location}/../Project/imgui.ini %{cfg.buildtarget.directory}/", -- UNCOMMENT THIS LINE IF USING Windows-Build.bat TO BUILD INSTEAD OF AN IDE
            "{COPY} %{wks.location}/../Project/Resources %{wks.location}/Build-Files/Resources", -- Comment this line out if you are not building via IDE
            "{COPY} %{wks.location}/../Project/imgui.ini %{wks.location}/Build-Files/", -- Comment this line out if you are not building via IDE
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
            "{COPY} %{wks.location}/../Project/Resources %{cfg.buildtarget.directory}/Resources",
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
