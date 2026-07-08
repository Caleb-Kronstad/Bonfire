#!/bin/bash

# Defaults
BUILD_SYSTEM=premake
BUILD_CONFIG=Debug

# Parse arguments
for arg in "$@"; do
    case $arg in
        cmake|premake) BUILD_SYSTEM=$arg ;;
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
    esac
done

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Premake uses lowercase system name (linux-x86_64), CMake uses CMAKE_SYSTEM_NAME (Linux-x86_64)
if [ "$BUILD_SYSTEM" = "cmake" ]; then
    OUTPUT_DIR="Linux-x86_64/$BUILD_CONFIG"
else
    OUTPUT_DIR="linux-x86_64/$BUILD_CONFIG"
fi

EXECUTABLE="$PROJECT_ROOT/Build/Binaries/$OUTPUT_DIR/Editor/Editor"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable not found at $EXECUTABLE"
    echo "Please build the project first using Linux-Build.sh $BUILD_SYSTEM"
    exit 1
fi

echo "=== Running Project ($BUILD_SYSTEM $BUILD_CONFIG build) ==="
cd "$(dirname "$EXECUTABLE")"  # Change to executable directory so Assets folder is found
"$EXECUTABLE"
