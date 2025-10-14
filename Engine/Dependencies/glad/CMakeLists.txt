project(GLAD LANGUAGES C CXX)

  # Source files (matching Build-GLAD.lua)
  set(GLAD_SOURCES
      src/glad.cpp
  )

  set(GLAD_HEADERS
      include/glad/glad.h
      include/KHR/khrplatform.h
  )

  # Create static library
  add_library(GLAD STATIC
      ${GLAD_SOURCES}
      ${GLAD_HEADERS}
  )

  # Include directories
  target_include_directories(GLAD PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

  # Set output directory
  set_target_properties(GLAD PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Build/Binaries/${OUTPUT_DIR}/Project/Dependencies/GLAD"
  )
