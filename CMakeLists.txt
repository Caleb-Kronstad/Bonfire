cmake_minimum_required(VERSION 3.20)
project(Bonfire)

# ── Configurations ────────────────────────────────────────────────────────────
# Add "Dist" as a custom config (multi-config generators like VS)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug, Release, Dist)" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "Dist")

# Dist = Release flags, no debug symbols
set(CMAKE_CXX_FLAGS_DIST          "${CMAKE_CXX_FLAGS_RELEASE}"          CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_DIST            "${CMAKE_C_FLAGS_RELEASE}"            CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DIST   "${CMAKE_EXE_LINKER_FLAGS_RELEASE}"   CACHE STRING "" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_DIST "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)
set(CMAKE_STATIC_LINKER_FLAGS_DIST "${CMAKE_STATIC_LINKER_FLAGS_RELEASE}" CACHE STRING "" FORCE)

# ── Architecture ───────────────────────────────────────────────────────────────
set(CMAKE_VS_PLATFORM_NAME "x64")

# ── Output directories ─────────────────────────────────────────────────────────
# Single-config generators (gmake, ninja)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}")

# Multi-config generators (Visual Studio)
foreach(CONFIG Debug Release Dist)
    string(TOUPPER ${CONFIG} U)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${U} "${CMAKE_SOURCE_DIR}/Build/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/${CONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${U} "${CMAKE_SOURCE_DIR}/Build/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/${CONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${U} "${CMAKE_SOURCE_DIR}/Build/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}/${CONFIG}")
endforeach()

# ── Platform-wide compile options ─────────────────────────────────────────────
if(UNIX AND NOT APPLE)
    add_compile_options(-fPIC)
    add_link_options(-pthread)
elseif(WIN32)
    add_compile_options(/EHsc /Zc:preprocessor /Zc:__cplusplus)
endif()

# ── Include directory variables (mirrors Premake IncludeDir table) ─────────────
set(INCLUDE_GLAD      "${CMAKE_SOURCE_DIR}/Engine/Dependencies/glad/include")
set(INCLUDE_GLFW      "${CMAKE_SOURCE_DIR}/Engine/Dependencies/glfw/include")
set(INCLUDE_IMGUI     "${CMAKE_SOURCE_DIR}/Engine/Dependencies/imgui")
set(INCLUDE_STB_IMAGE "${CMAKE_SOURCE_DIR}/Engine/Dependencies/stb_image")
set(INCLUDE_ASSIMP    "${CMAKE_SOURCE_DIR}/Engine/Dependencies/assimp/include")
set(INCLUDE_GLM       "${CMAKE_SOURCE_DIR}/Engine/Dependencies/glm/glm")
set(INCLUDE_IMGUIZMO  "${CMAKE_SOURCE_DIR}/Engine/Dependencies/imguizmo")
set(INCLUDE_JSON      "${CMAKE_SOURCE_DIR}/Engine/Dependencies/json")
set(INCLUDE_JOLT      "${CMAKE_SOURCE_DIR}/Engine/Dependencies/jolt")
set(INCLUDE_ASIO      "${CMAKE_SOURCE_DIR}/Engine/Dependencies/asio/include")
set(INCLUDE_LUA       "${CMAKE_SOURCE_DIR}/Engine/Dependencies/lua-5.4.8/src")
set(INCLUDE_MINIAUDIO "${CMAKE_SOURCE_DIR}/Engine/Dependencies/miniaudio")

# ── Dependencies ───────────────────────────────────────────────────────────────
add_subdirectory(Engine/Dependencies/glad)
add_subdirectory(Engine/Dependencies/glfw)
add_subdirectory(Engine/Dependencies/imgui)
add_subdirectory(Engine/Dependencies/assimp)
add_subdirectory(Engine/Dependencies/glm)
add_subdirectory(Engine/Dependencies/imguizmo)
add_subdirectory(Engine/Dependencies/jolt)
add_subdirectory(Engine/Dependencies/lua-5.4.8)

# ── Projects ───────────────────────────────────────────────────────────────────
add_subdirectory(Engine)
add_subdirectory(Editor)
