project "LUA"
      location "../../../Build/Build-Files"
      kind "StaticLib"
      language "C"
      staticruntime "on"

      targetdir ("../../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
      objdir ("../../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

      files {
          "src/*.h",
          "src/*.c"
      }

      removefiles {
          "src/lua.c",
          "src/luac.c"
      }

      includedirs {
          "src"
      }

      filter "system:windows"
          systemversion "latest"
          defines {
              "_CRT_SECURE_NO_WARNINGS",
              "LUA_USE_WINDOWS"
          }

      filter "system:linux"
          pic "On"
          defines {
              "LUA_USE_LINUX"
          }
          links {
              "dl",
              "m"
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