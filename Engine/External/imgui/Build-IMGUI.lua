project "IMGUI"
	kind "StaticLib"
	language "C++"

	targetdir ("../../../bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("../../../bin-int/" .. OutputDir .. "/%{prj.name}")

	includedirs { 
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.GLAD}",
        "%{IncludeDir.GLFW}"
	}

	files {
		"*.cpp",
		"*.h",
		"misc/cpp/*.cpp",
		"misc/cpp/*.h",
		"backends/imgui_impl_glfw.h",
		"backends/imgui_impl_opengl3.h",
		"backends/imgui_impl_glfw.cpp",
		"backends/imgui_impl_opengl3.cpp"
	}

	defines {
		"IMGUI_IMPL_OPENGL_LOADER_GLAD"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines { 
			"_IMGUI_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
    
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		defines	{
			"_IMGUI_X11"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"