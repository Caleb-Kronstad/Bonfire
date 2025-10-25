project "GLAD"
    location "../../../Build/Build-Files"
    kind "StaticLib"
    language "C++"

    targetdir ("../../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.cpp"
    }

    includedirs {
        "include"
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"

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
