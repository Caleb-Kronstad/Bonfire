project "IMGUIZMO"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("../../../bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("../../../bin-int/" .. OutputDir .. "/%{prj.name}")

	includedirs { 
        "%{IncludeDir.IMGUI}"
	}

	files {
		"ImGradient.cpp",
		"ImGradient.h",
		"ImCurveEdit.cpp",
		"ImCurveEdit.h",
		"ImGuizmo.cpp",
		"ImGuizmo.h",
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

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"