#!/bin/bash

  # Get the directory where this script is located
  SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
  PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

  source "$SCRIPT_DIR/Linux-Config.sh"

  # Parse build system argument (default to premake)
  BUILD_SYSTEM=${1:-premake}

  # Both CMake and Premake use capitalized system name
  OUTPUT_DIR="Linux-x86_64/$BUILD_CONFIG"

  EXECUTABLE="$PROJECT_ROOT/Build/Binaries/$OUTPUT_DIR/Editor/Editor"

  if [ ! -f "$EXECUTABLE" ]; then
      echo "Error: Executable not found at $EXECUTABLE"
      echo "Please build the project first using Linux-Build.sh $BUILD_SYSTEM"
      exit 1
  fi

  echo "=== Running Project ($BUILD_SYSTEM build) ==="
  cd "$(dirname "$EXECUTABLE")"  # Change to executable directory so Assets folder is found
  "$EXECUTABLE"
