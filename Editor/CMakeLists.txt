file(GLOB_RECURSE EDITOR_SOURCES
    "Source/**"
    "Data/**"
)

add_executable(Editor ${EDITOR_SOURCES})

target_include_directories(Editor PRIVATE
    Source
    Data
    ${CMAKE_SOURCE_DIR}/Engine/Source
    ${INCLUDE_GLAD}
    ${INCLUDE_GLFW}
    ${INCLUDE_IMGUI}
    ${INCLUDE_STB_IMAGE}
    ${INCLUDE_ASSIMP}
    ${INCLUDE_GLM}
    ${INCLUDE_IMGUIZMO}
    ${INCLUDE_JSON}
    ${INCLUDE_JOLT}
    ${INCLUDE_LUA}
    ${INCLUDE_MINIAUDIO}
)

target_link_libraries(Editor PRIVATE Engine GLAD GLFW IMGUI ASSIMP GLM IMGUIZMO JOLT LUA)

target_compile_definitions(Editor PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:Dist>:DIST>
)

if(UNIX AND NOT APPLE)
    target_compile_definitions(Editor PRIVATE BONFIRE_PLATFORM_LINUX)
    target_link_libraries(Editor PRIVATE GL dl pthread X11 Xrandr Xi Xxf86vm Xcursor Xinerama)
elseif(WIN32)
    target_compile_definitions(Editor PRIVATE
        BONFIRE_PLATFORM_WINDOWS
        _CRT_SECURE_NO_WARNINGS
    )
endif()

set_target_properties(Editor PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
