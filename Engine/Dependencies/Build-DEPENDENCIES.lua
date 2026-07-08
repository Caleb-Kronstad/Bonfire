project "GLAD"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    staticruntime "On"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    files {
        "glad/include/glad/glad.h",
        "glad/include/KHR/khrplatform.h",
        "glad/src/glad.cpp"
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
        "glfw/src/osmesa_context.c"
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
            "glfw/src/wgl_context.c"
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
            "glfw/src/linux_joystick.c"
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

project "ASSIMP"
    location "../../Build/Build-Files"
    kind "StaticLib"
    language "C++"
    cppdialect "C++11"
    staticruntime "on"

    targetdir ("../../Build/Binaries/" .. OutputDir .. "/Dependencies/%{prj.name}")
    objdir ("../../Build/Binaries-Intermediate/" .. OutputDir .. "/Dependencies/%{prj.name}")

    defines {
        -- "SWIG",
        -- "ASSIMP_BUILD_NO_OWN_ZLIB",

        "ASSIMP_BUILD_NO_X_IMPORTER",
        "ASSIMP_BUILD_NO_3DS_IMPORTER",
        "ASSIMP_BUILD_NO_MD3_IMPORTER",
        "ASSIMP_BUILD_NO_MDL_IMPORTER",
        "ASSIMP_BUILD_NO_MD2_IMPORTER",
        -- "ASSIMP_BUILD_NO_PLY_IMPORTER",
        "ASSIMP_BUILD_NO_ASE_IMPORTER",
        -- "ASSIMP_BUILD_NO_OBJ_IMPORTER",
        "ASSIMP_BUILD_NO_AMF_IMPORTER",
        "ASSIMP_BUILD_NO_HMP_IMPORTER",
        "ASSIMP_BUILD_NO_SMD_IMPORTER",
        "ASSIMP_BUILD_NO_MDC_IMPORTER",
        "ASSIMP_BUILD_NO_MD5_IMPORTER",
        "ASSIMP_BUILD_NO_STL_IMPORTER",
        "ASSIMP_BUILD_NO_LWO_IMPORTER",
        "ASSIMP_BUILD_NO_DXF_IMPORTER",
        "ASSIMP_BUILD_NO_NFF_IMPORTER",
        "ASSIMP_BUILD_NO_RAW_IMPORTER",
        "ASSIMP_BUILD_NO_OFF_IMPORTER",
        "ASSIMP_BUILD_NO_AC_IMPORTER",
        "ASSIMP_BUILD_NO_BVH_IMPORTER",
        "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
        "ASSIMP_BUILD_NO_IRR_IMPORTER",
        "ASSIMP_BUILD_NO_Q3D_IMPORTER",
        "ASSIMP_BUILD_NO_B3D_IMPORTER",
        -- "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
        "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
        "ASSIMP_BUILD_NO_CSM_IMPORTER",
        "ASSIMP_BUILD_NO_3D_IMPORTER",
        "ASSIMP_BUILD_NO_LWS_IMPORTER",
        "ASSIMP_BUILD_NO_OGRE_IMPORTER",
        "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
        "ASSIMP_BUILD_NO_MS3D_IMPORTER",
        "ASSIMP_BUILD_NO_COB_IMPORTER",
        "ASSIMP_BUILD_NO_BLEND_IMPORTER",
        "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
        "ASSIMP_BUILD_NO_NDO_IMPORTER",
        "ASSIMP_BUILD_NO_IFC_IMPORTER",
        "ASSIMP_BUILD_NO_XGL_IMPORTER",
        -- "ASSIMP_BUILD_NO_FBX_IMPORTER",
        "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
        -- "ASSIMP_BUILD_NO_GLTF_IMPORTER",
        "ASSIMP_BUILD_NO_C4D_IMPORTER",
        "ASSIMP_BUILD_NO_3MF_IMPORTER",
        "ASSIMP_BUILD_NO_X3D_IMPORTER",
        "ASSIMP_BUILD_NO_MMD_IMPORTER",

        "ASSIMP_BUILD_NO_STEP_EXPORTER",
        "ASSIMP_BUILD_NO_SIB_IMPORTER",

        -- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
        -- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
        -- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
        -- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
        "ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
        -- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
        "ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
        -- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",
        "ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
        "ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
        "ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
        "ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
        -- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
        "ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
        "ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
        "ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
        "ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
        "ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
        "ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
        "ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
        "ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
        "ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
        "ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
        "ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
        "ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
        "ASSIMP_BUILD_NO_DEBONE_PROCESS",
        "ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
        "ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS",
    }

    files {
        "assimp/include/**",
        "assimp/code/Assimp.cpp",
        "assimp/code/BaseImporter.cpp",
        "assimp/code/ColladaLoader.cpp",
        "assimp/code/ColladaParser.cpp",
        "assimp/code/CreateAnimMesh.cpp",
        "assimp/code/PlyParser.cpp",
        "assimp/code/PlyLoader.cpp",
        "assimp/code/BaseProcess.cpp",
        "assimp/code/EmbedTexturesProcess.cpp",
        "assimp/code/ConvertToLHProcess.cpp",
        "assimp/code/DefaultIOStream.cpp",
        "assimp/code/DefaultIOSystem.cpp",
        "assimp/code/DefaultLogger.cpp",
        "assimp/code/GenVertexNormalsProcess.cpp",
        "assimp/code/Importer.cpp",
        "assimp/code/ImporterRegistry.cpp",
        "assimp/code/MaterialSystem.cpp",
        "assimp/code/PostStepRegistry.cpp",
        "assimp/code/ProcessHelper.cpp",
        "assimp/code/scene.cpp",
        "assimp/code/ScenePreprocessor.cpp",
        "assimp/code/ScaleProcess.cpp",
        "assimp/code/SGSpatialSort.cpp",
        "assimp/code/SkeletonMeshBuilder.cpp",
        "assimp/code/SpatialSort.cpp",
        "assimp/code/TriangulateProcess.cpp",
        "assimp/code/ValidateDataStructure.cpp",
        "assimp/code/Version.cpp",
        "assimp/code/VertexTriangleAdjacency.cpp",
        "assimp/code/ObjFileImporter.cpp",
        "assimp/code/ObjFileMtlImporter.cpp",
        "assimp/code/ObjFileParser.cpp",
        "assimp/code/glTFImporter.cpp",
        "assimp/code/glTF2Importer.cpp",
        "assimp/code/MakeVerboseFormat.cpp",
        "assimp/code/CalcTangentsProcess.cpp",
        "assimp/code/FBXAnimation.cpp",
        "assimp/code/FBXBinaryTokenizer.cpp",
        "assimp/code/FBXConverter.cpp",
        "assimp/code/FBXDeformer.cpp",
        "assimp/code/FBXDocument.cpp",
        "assimp/code/FBXDocumentUtil.cpp",
        "assimp/code/FBXImporter.cpp",
        "assimp/code/FBXMaterial.cpp",
        "assimp/code/FBXMeshGeometry.cpp",
        "assimp/code/FBXModel.cpp",
        "assimp/code/FBXNodeAttribute.cpp",
        "assimp/code/FBXParser.cpp",
        "assimp/code/FBXProperties.cpp",
        "assimp/code/FBXTokenizer.cpp",
        "assimp/code/FBXUtil.cpp",
        "assimp/contrib/zlib/adler32.c",
        "assimp/contrib/zlib/compress.c",
        "assimp/contrib/zlib/crc32.c",
        "assimp/contrib/zlib/deflate.c",
        "assimp/contrib/zlib/infback.c",
        "assimp/contrib/zlib/inffast.c",
        "assimp/contrib/zlib/inflate.c",
        "assimp/contrib/zlib/inftrees.c",
        "assimp/contrib/zlib/trees.c",
        "assimp/contrib/zlib/uncompr.c",
        "assimp/contrib/zlib/zutil.c",
        "assimp/code/ScaleProcess.cpp",
        "assimp/code/EmbedTexturesProcess.cpp",
        "assimp/contrib/irrXML/*",
    }

    includedirs {
        "assimp/include",
        "assimp/contrib/irrXML",
        "assimp/contrib/zlib",
        "assimp/contrib/rapidjson/include",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:linux"
        pic "On"
        buildoptions { "-Wno-unused-variable", "-Wno-unused-but-set-variable" }

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
        "glm/glm/placeholder.cpp",
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
        "imguizmo/ImGradient.cpp",
        "imguizmo/ImGradient.h",
        "imguizmo/ImCurveEdit.cpp",
        "imguizmo/ImCurveEdit.h",
        "imguizmo/ImGuizmo.cpp",
        "imguizmo/ImGuizmo.h",
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
