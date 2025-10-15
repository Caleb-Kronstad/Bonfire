cmake_minimum_required(VERSION 3.15)
  project(Bonfire VERSION 1.0 LANGUAGES C CXX)

  # Set C++ standard
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Set C standard for C dependencies
  set(CMAKE_C_STANDARD 11)

  # Default build type
  if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE Debug)
  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      set(SYSTEM_NAME_CAPITALIZED "linux")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set(SYSTEM_NAME_CAPITALIZED "windows")
  else()
      set(SYSTEM_NAME_CAPITALIZED ${CMAKE_SYSTEM_NAME})
  endif()
  set(OUTPUT_DIR "${SYSTEM_NAME_CAPITALIZED}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${OUTPUT_DIR}/Project")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${OUTPUT_DIR}/Project")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${OUTPUT_DIR}/Project")

  # Set PIC for all targets
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # Platform-specific settings
  if(WIN32)
      add_compile_definitions(BONFIRE_PLATFORM_WINDOWS _CRT_SECURE_NO_WARNINGS)
      # MSVC-specific flags
      if(MSVC)
          add_compile_options(/EHsc /Zc:preprocessor /Zc:__cplusplus)
      endif()
  elseif(UNIX AND NOT APPLE)
      add_compile_definitions(BONFIRE_PLATFORM_LINUX)
      add_compile_options(-fPIC)
      link_libraries(pthread)
  endif()

  # Configuration-specific defines
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_compile_definitions(DEBUG)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
      add_compile_definitions(RELEASE)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Dist")
      add_compile_definitions(DIST)
  endif()

  # Enable ccache if available
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
      set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
      set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
      message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
  endif()

  # Add subdirectories
  add_subdirectory(Engine/Dependencies/glad)
  add_subdirectory(Engine/Dependencies/glfw)
  add_subdirectory(Engine/Dependencies/imgui)
  add_subdirectory(Engine/Dependencies/assimp)
  add_subdirectory(Engine/Dependencies/glm)
  add_subdirectory(Engine/Dependencies/imguizmo)
  add_subdirectory(Engine)
  add_subdirectory(Project)
