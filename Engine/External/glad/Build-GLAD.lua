project "GLAD"
    kind "StaticLib"
    language "C++"

	targetdir ("../../../bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("../../../bin-int/" .. OutputDir .. "/%{prj.name}")

	files {
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.cpp"
	}

    includedirs {
        "include"
    }

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}
    
	filter "system:windows"
		systemversion "latest"