project(Engine LANGUAGES CXX)

  # Collect source files
  file(GLOB_RECURSE ENGINE_SOURCES
      "Source/*.cpp"
      "Source/*.c"
  )

  file(GLOB_RECURSE ENGINE_HEADERS
      "Source/*.h"
      "Source/*.hpp"
  )

  # Add STB_IMAGE and JSON sources
  file(GLOB STB_IMAGE_SOURCES
      "Dependencies/stb_image/*.cpp"
      "Dependencies/stb_image/*.h"
  )

  # Create static library
  add_library(Engine STATIC
      ${ENGINE_SOURCES}
      ${ENGINE_HEADERS}
      ${STB_IMAGE_SOURCES}
  )

  # Include directories
  target_include_directories(Engine PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/Source
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glad/include
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glfw/include
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/imgui
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb_image
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm/glm
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/imguizmo
      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/json
  )

  # Link dependencies
  target_link_libraries(Engine PUBLIC
      GLAD
      GLFW
      IMGUI
      assimp
      GLM
      IMGUIZMO
  )

  # Platform-specific linking
  if(WIN32)
      target_link_libraries(Engine PUBLIC opengl32 comdlg32)
      target_compile_definitions(Engine PUBLIC BONFIRE_BUILD_DLL)
  elseif(UNIX AND NOT APPLE)
      target_link_libraries(Engine PUBLIC GL dl pthread)
  endif()

  # Precompiled header
  target_precompile_headers(Engine PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/Source/bonfire_pch.hpp
  )

  # Set output directory
  set_target_properties(Engine PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${OUTPUT_DIR}/Project/Engine"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${OUTPUT_DIR}/Project/Engine"
  )
