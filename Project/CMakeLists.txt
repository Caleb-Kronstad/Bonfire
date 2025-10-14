project(Project LANGUAGES CXX)

  # Collect source files
  file(GLOB_RECURSE PROJECT_SOURCES
      "Source/*.cpp"
      "Source/*.c"
  )

  file(GLOB_RECURSE PROJECT_HEADERS
      "Source/*.h"
      "Source/*.hpp"
  )

  # Create executable
  add_executable(Project
      ${PROJECT_SOURCES}
      ${PROJECT_HEADERS}
  )

  # Include directories
  target_include_directories(Project PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/Assets
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Source
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/glad/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/glfw/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/imgui
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/stb_image
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/assimp/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/glm
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/imguizmo
      ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Dependencies/json/include
  )

  # Link libraries
  target_link_libraries(Project PRIVATE
      Engine
      GLAD
      GLFW
      IMGUI
      assimp
      GLM
      IMGUIZMO
  )

  # Platform-specific linking
  if(UNIX AND NOT APPLE)
      target_link_libraries(Project PRIVATE
          GL dl pthread X11 Xrandr Xi Xxf86vm Xcursor Xinerama
      )
  endif()

  # Post-build: Copy assets
  add_custom_command(TARGET Project POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_CURRENT_SOURCE_DIR}/Assets
          $<TARGET_FILE_DIR:Project>/Assets
      COMMENT "Copying assets to build directory"
  )

  # Post-build: Copy imgui.ini
  add_custom_command(TARGET Project POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini
          $<TARGET_FILE_DIR:Project>/imgui.ini
      COMMENT "Copying imgui.ini to build directory"
  )

