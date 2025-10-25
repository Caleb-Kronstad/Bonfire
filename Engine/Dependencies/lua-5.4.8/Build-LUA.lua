project "LUA"
    location "../../../Build/Build-Files"
    kind "StaticLib"
    language "C"

    targetdir ("../../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "src/lapi.c",
        "src/lcode.c",
        "src/lctype.c",
        "src/ldebug.c",
        "src/ldo.c",
        "src/ldump.c",
        "src/lfunc.c",
        "src/lgc.c",
        "src/llex.c",
        "src/lmem.c",
        "src/lobject.c",
        "src/lopcodes.c",
        "src/lparser.c",
        "src/lstate.c",
        "src/lstring.c",
        "src/ltable.c",
        "src/ltm.c",
        "src/lundump.c",
        "src/lvm.c",
        "src/lzio.c",
        "src/lauxlib.c",
        "src/lbaselib.c",
        "src/lcorolib.c",
        "src/ldblib.c",
        "src/liolib.c",
        "src/lmathlib.c",
        "src/loadlib.c",
        "src/loslib.c",
        "src/lstrlib.c",
        "src/ltablib.c",
        "src/lutf8lib.c",
        "src/linit.c",
        "src/**.h"
    }

    includedirs {
        "src"
    }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "LUA_COMPAT_5_3"
        }

    filter "system:linux"
        pic "On"
        staticruntime "On"
        defines {
            "LUA_USE_LINUX",
            "LUA_COMPAT_5_3"
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