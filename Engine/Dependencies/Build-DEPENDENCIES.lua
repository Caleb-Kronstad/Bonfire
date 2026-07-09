project "GLAD"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C"
    staticruntime "On"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "glad/include/glad/glad.h",
        "glad/include/KHR/khrplatform.h",
        "glad/src/glad.c"
    }

    includedirs {
        "glad/include"
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

project "SOIL"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C"
    staticruntime "On"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "soil/src/SOIL2/*.c",
        "soil/src/SOIL2/*.h"
    }

    includedirs {
        "soil/src/SOIL2"
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

project "GLFW"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "glfw/include/GLFW/glfw3.h",
        "glfw/include/GLFW/glfw3native.h",
        "glfw/src/glfw_config.h",
        "glfw/src/context.c",
        "glfw/src/init.c",
        "glfw/src/input.c",
        "glfw/src/monitor.c",
        "glfw/src/vulkan.c",
        "glfw/src/window.c",
        "glfw/src/egl_context.c",
        "glfw/src/osmesa_context.c",
        "glfw/src/platform.c",
        "glfw/src/null_init.c",
        "glfw/src/null_joystick.c",
        "glfw/src/null_monitor.c",
        "glfw/src/null_window.c"
    }

    includedirs {
        "glfw/include"
    }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"

        files {
            "glfw/src/win32_init.c",
            "glfw/src/win32_joystick.c",
            "glfw/src/win32_monitor.c",
            "glfw/src/win32_time.c",
            "glfw/src/win32_thread.c",
            "glfw/src/win32_window.c",
            "glfw/src/wgl_context.c",
            "glfw/src/win32_module.c"
        }

        defines {
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"
        staticruntime "On"

        files {
            "glfw/src/x11_init.c",
            "glfw/src/x11_monitor.c",
            "glfw/src/x11_window.c",
            "glfw/src/xkb_unicode.c",
            "glfw/src/posix_time.c",
            "glfw/src/posix_thread.c",
            "glfw/src/glx_context.c",
            "glfw/src/linux_joystick.c",
            "glfw/src/posix_module.c",
            "glfw/src/posix_poll.c"
        }

        defines {
            "_GLFW_X11"
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

project "IMGUI"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    includedirs {
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}"
    }

    files {
        "imgui/*.cpp",
        "imgui/*.h",
        "imgui/misc/cpp/*.cpp",
        "imgui/misc/cpp/*.h",
        "imgui/backends/imgui_impl_glfw.h",
        "imgui/backends/imgui_impl_opengl3.h",
        "imgui/backends/imgui_impl_glfw.cpp",
        "imgui/backends/imgui_impl_opengl3.cpp"
    }

    defines {
        "IMGUI_IMPL_OPENGL_LOADER_GLAD"
    }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"
        staticruntime "On"

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

-- ASSIMP: only Obj/Collada/Ply/glTF+glTF2/FBX importers are compiled (matches the
-- formats this engine actually loads). Everything else is disabled via
-- ASSIMP_BUILD_NO_<FORMAT>_IMPORTER, which ImporterRegistry.cpp checks to skip
-- referencing the corresponding (uncompiled) importer classes. Exporters are not
-- compiled at all (Common/Exporter.cpp is omitted) since nothing here exports.
project "ASSIMP"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    defines {
        "ASSIMP_BUILD_NO_AMF_IMPORTER",
        "ASSIMP_BUILD_NO_3DS_IMPORTER",
        "ASSIMP_BUILD_NO_AC_IMPORTER",
        "ASSIMP_BUILD_NO_ASE_IMPORTER",
        "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
        "ASSIMP_BUILD_NO_B3D_IMPORTER",
        "ASSIMP_BUILD_NO_BVH_IMPORTER",
        "ASSIMP_BUILD_NO_DXF_IMPORTER",
        "ASSIMP_BUILD_NO_CSM_IMPORTER",
        "ASSIMP_BUILD_NO_HMP_IMPORTER",
        "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
        "ASSIMP_BUILD_NO_IQM_IMPORTER",
        "ASSIMP_BUILD_NO_IRR_IMPORTER",
        "ASSIMP_BUILD_NO_LWO_IMPORTER",
        "ASSIMP_BUILD_NO_LWS_IMPORTER",
        "ASSIMP_BUILD_NO_M3D_IMPORTER",
        "ASSIMP_BUILD_NO_MD2_IMPORTER",
        "ASSIMP_BUILD_NO_MD3_IMPORTER",
        "ASSIMP_BUILD_NO_MD5_IMPORTER",
        "ASSIMP_BUILD_NO_MDC_IMPORTER",
        "ASSIMP_BUILD_NO_MDL_IMPORTER",
        "ASSIMP_BUILD_NO_NFF_IMPORTER",
        "ASSIMP_BUILD_NO_NDO_IMPORTER",
        "ASSIMP_BUILD_NO_OFF_IMPORTER",
        "ASSIMP_BUILD_NO_OGRE_IMPORTER",
        "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
        "ASSIMP_BUILD_NO_MS3D_IMPORTER",
        "ASSIMP_BUILD_NO_COB_IMPORTER",
        "ASSIMP_BUILD_NO_BLEND_IMPORTER",
        "ASSIMP_BUILD_NO_IFC_IMPORTER",
        "ASSIMP_BUILD_NO_XGL_IMPORTER",
        "ASSIMP_BUILD_NO_Q3D_IMPORTER",
        "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
        "ASSIMP_BUILD_NO_RAW_IMPORTER",
        "ASSIMP_BUILD_NO_SIB_IMPORTER",
        "ASSIMP_BUILD_NO_SMD_IMPORTER",
        "ASSIMP_BUILD_NO_STL_IMPORTER",
        "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
        "ASSIMP_BUILD_NO_3D_IMPORTER",
        "ASSIMP_BUILD_NO_USD_IMPORTER",
        "ASSIMP_BUILD_NO_X_IMPORTER",
        "ASSIMP_BUILD_NO_X3D_IMPORTER",
        "ASSIMP_BUILD_NO_3MF_IMPORTER",
        "ASSIMP_BUILD_NO_MMD_IMPORTER",
        "ASSIMP_BUILD_NO_C4D_IMPORTER",

        "ASSIMP_BUILD_NO_EXPORT",
    }

    files {
        "assimp/include/**",

        -- Core / Common / infra (always required)
        "assimp/code/Common/Assimp.cpp",
        "assimp/code/Common/DefaultLogger.cpp",
        "assimp/code/Common/Compression.cpp",
        "assimp/code/Common/BaseImporter.cpp",
        "assimp/code/Common/BaseProcess.cpp",
        "assimp/code/Common/PostStepRegistry.cpp",
        "assimp/code/Common/ImporterRegistry.cpp",
        "assimp/code/Common/DefaultIOStream.cpp",
        "assimp/code/Common/IOSystem.cpp",
        "assimp/code/Common/DefaultIOSystem.cpp",
        "assimp/code/Common/ZipArchiveIOSystem.cpp",
        "assimp/code/Common/Importer.cpp",
        "assimp/code/Common/SGSpatialSort.cpp",
        "assimp/code/Common/VertexTriangleAdjacency.cpp",
        "assimp/code/Common/SpatialSort.cpp",
        "assimp/code/Common/SceneCombiner.cpp",
        "assimp/code/Common/ScenePreprocessor.cpp",
        "assimp/code/Common/SkeletonMeshBuilder.cpp",
        "assimp/code/Common/StandardShapes.cpp",
        "assimp/code/Common/TargetAnimation.cpp",
        "assimp/code/Common/RemoveComments.cpp",
        "assimp/code/Common/Subdivision.cpp",
        "assimp/code/Common/scene.cpp",
        "assimp/code/Common/Bitmap.cpp",
        "assimp/code/Common/Version.cpp",
        "assimp/code/Common/CreateAnimMesh.cpp",
        "assimp/code/Common/simd.cpp",
        "assimp/code/Common/material.cpp",
        "assimp/code/Common/AssertHandler.cpp",
        "assimp/code/Common/Exceptional.cpp",
        "assimp/code/Common/Base64.cpp",
        "assimp/code/CApi/CInterfaceIOWrapper.cpp",
        "assimp/code/Geometry/GeometryUtils.cpp",
        "assimp/code/Material/MaterialSystem.cpp",

        -- PostProcessing (format-independent, always compiled)
        "assimp/code/PostProcessing/*.cpp",

        -- glTF shared asset code (used by both glTF and glTF2 importers)
        "assimp/code/AssetLib/glTFCommon/glTFCommon.cpp",

        -- Obj
        "assimp/code/AssetLib/Obj/ObjFileImporter.cpp",
        "assimp/code/AssetLib/Obj/ObjFileMtlImporter.cpp",
        "assimp/code/AssetLib/Obj/ObjFileParser.cpp",

        -- Collada
        "assimp/code/AssetLib/Collada/ColladaHelper.cpp",
        "assimp/code/AssetLib/Collada/ColladaLoader.cpp",
        "assimp/code/AssetLib/Collada/ColladaParser.cpp",

        -- Ply
        "assimp/code/AssetLib/Ply/PlyLoader.cpp",
        "assimp/code/AssetLib/Ply/PlyParser.cpp",

        -- glTF / glTF2
        "assimp/code/AssetLib/glTF/glTFImporter.cpp",
        "assimp/code/AssetLib/glTF2/glTF2Importer.cpp",

        -- FBX
        "assimp/code/AssetLib/FBX/FBXUtil.cpp",
        "assimp/code/AssetLib/FBX/FBXAnimation.cpp",
        "assimp/code/AssetLib/FBX/FBXBinaryTokenizer.cpp",
        "assimp/code/AssetLib/FBX/FBXConverter.cpp",
        "assimp/code/AssetLib/FBX/FBXDeformer.cpp",
        "assimp/code/AssetLib/FBX/FBXDocument.cpp",
        "assimp/code/AssetLib/FBX/FBXDocumentUtil.cpp",
        "assimp/code/AssetLib/FBX/FBXImporter.cpp",
        "assimp/code/AssetLib/FBX/FBXMaterial.cpp",
        "assimp/code/AssetLib/FBX/FBXMeshGeometry.cpp",
        "assimp/code/AssetLib/FBX/FBXModel.cpp",
        "assimp/code/AssetLib/FBX/FBXNodeAttribute.cpp",
        "assimp/code/AssetLib/FBX/FBXParser.cpp",
        "assimp/code/AssetLib/FBX/FBXProperties.cpp",
        "assimp/code/AssetLib/FBX/FBXTokenizer.cpp",

        -- Third-party (only what OBJ/Collada/Ply/glTF/glTF2/FBX + Common actually need:
        -- pugixml for Collada's XmlParser, zlib for Compression/FBX, unzip for
        -- ZipArchiveIOSystem, rapidjson/utf8cpp are header-only)
        "assimp/contrib/pugixml/src/pugixml.cpp",
        "assimp/contrib/zlib/*.c",
        "assimp/contrib/unzip/unzip.c",
        "assimp/contrib/unzip/ioapi.c",
    }

    includedirs {
        "assimp/include",
        "assimp/code",
        "assimp",
        "assimp/contrib/pugixml/src",
        "assimp/contrib/rapidjson/include",
        "assimp/contrib/utf8cpp/source",
        "assimp/contrib/zlib",
        "assimp/contrib/unzip",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"
        defines { "HAVE_UNISTD_H" }
        buildoptions { "-Wno-unused-variable", "-Wno-unused-but-set-variable" }

    filter { "system:linux", "files:**.c" }
        buildoptions { "-Wno-old-style-definition", "-Wno-implicit-function-declaration" }

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

project "GLM"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "glm/glm/common.hpp",
        "glm/glm/exponential.hpp",
        "glm/glm/ext.hpp",
        "glm/glm/fwd.hpp",
        "glm/glm/geometric.hpp",
        "glm/glm/glm.hpp",
        "glm/glm/integer.hpp",
        "glm/glm/mat2x2.hpp",
        "glm/glm/mat2x3.hpp",
        "glm/glm/mat2x4.hpp",
        "glm/glm/mat3x2.hpp",
        "glm/glm/mat3x3.hpp",
        "glm/glm/mat3x4.hpp",
        "glm/glm/mat4x2.hpp",
        "glm/glm/mat4x3.hpp",
        "glm/glm/mat4x4.hpp",
        "glm/glm/matrix.hpp",
        "glm/glm/packing.hpp",
        "glm/glm/trigonometric.hpp",
        "glm/glm/vec2.hpp",
        "glm/glm/vec3.hpp",
        "glm/glm/vec4.hpp",
        "glm/glm/vector_relational.hpp",
        "glm/glm/gtc/**cpp",
        "glm/glm/gtc/**hpp",
        "glm/glm/gtx/**cpp",
        "glm/glm/gtx/**hpp",
    }

    defines {
        "GLM_ENABLE_EXPERIMENTAL"
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

project "IMGUIZMO"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    includedirs {
        "%{IncludeDir.IMGUI}"
    }

    files {
        "imguizmo/src/ImGradient.cpp",
        "imguizmo/src/ImGradient.h",
        "imguizmo/src/ImCurveEdit.cpp",
        "imguizmo/src/ImCurveEdit.h",
        "imguizmo/src/ImGuizmo.cpp",
        "imguizmo/src/ImGuizmo.h",
    }

    defines {
        "IMGUI_DEFINE_MATH_OPERATORS"
    }

    filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"
        staticruntime "On"

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

project "JOLT"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
          "jolt/Jolt/**.h",
          "jolt/Jolt/**.cpp",
          "jolt/Jolt/**.inl"
    }

    includedirs {
        "jolt"
    }

    defines {
        -- "JPH_OBJECT_STREAM",
        "JPH_DEBUG_RENDERER",
        "JPH_PROFILE_ENABLED",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
        }

    filter "system:linux"
        pic "On"
        buildoptions {
            "-Wno-unused-variable",
            "-Wno-unused-but-set-variable",
            "-Wno-comment"
        }

    filter "configurations:Debug"
        defines {
            "DEBUG",
            "JPH_PROFILE_ENABLED",
            "JPH_DEBUG_RENDERER",
            "JPH_ENABLE_ASSERTS"
        }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines {
            "RELEASE",
            "JPH_ENABLE_ASSERTS"
        }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines {
            "DIST",
            "NDEBUG"
        }
        runtime "Release"
        optimize "On"
        symbols "Off"

project "LUA"
      location "../../Build/Build-Files"
      kind "StaticLib"
      language "C"
      staticruntime "on"

      targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
      objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

      files {
          "lua-5.4.8/src/*.h",
          "lua-5.4.8/src/*.c"
      }

      removefiles {
          "lua-5.4.8/src/lua.c",
          "lua-5.4.8/src/luac.c"
      }

      includedirs {
          "lua-5.4.8/src"
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
