file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "Source/*.h"
    "Source/*.hpp"
    "Source/*.cpp"
    "Source/*.c"
)

add_library(Engine STATIC
    ${ENGINE_SOURCES}
    ${INCLUDE_STB_IMAGE}/stb_image.h
    ${INCLUDE_STB_IMAGE}/stb_image.cpp
    ${INCLUDE_MINIAUDIO}/miniaudio.h
    ${INCLUDE_MINIAUDIO}/miniaudio.c
)

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${INCLUDE_GLAD}
    ${INCLUDE_GLFW}
    ${INCLUDE_IMGUI}
    ${INCLUDE_STB_IMAGE}
    ${INCLUDE_ASSIMP}
    ${INCLUDE_GLM}
    ${INCLUDE_IMGUIZMO}
    ${INCLUDE_JSON}
    ${INCLUDE_JOLT}
    ${INCLUDE_ASIO}
    ${INCLUDE_LUA}
    ${INCLUDE_MINIAUDIO}
)

target_link_libraries(Engine PUBLIC GLAD GLFW IMGUI ASSIMP GLM IMGUIZMO JOLT LUA)

target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:Dist>:DIST>
)

if(UNIX AND NOT APPLE)
    target_compile_definitions(Engine PUBLIC BONFIRE_PLATFORM_LINUX)
    target_link_libraries(Engine PUBLIC GL dl pthread)
    target_precompile_headers(Engine PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Source/bonfire_pch.hpp>")
elseif(WIN32)
    target_compile_definitions(Engine PUBLIC
        BONFIRE_PLATFORM_WINDOWS
        BONFIRE_BUILD_DLL
        _CRT_SECURE_NO_WARNINGS
    )
    target_link_libraries(Engine PUBLIC opengl32 comdlg32)
    target_precompile_headers(Engine PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Source/bonfire_pch.hpp>")
endif()

set_target_properties(Engine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)
