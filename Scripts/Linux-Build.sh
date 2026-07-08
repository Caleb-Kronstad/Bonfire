#!/bin/bash

set -e

# Defaults
BUILD_SYSTEM=premake
BUILD_CONFIG=Debug
USE_BEAR=false
CLEAN_BUILD=false

# Parse arguments
for arg in "$@"; do
    case $arg in
        cmake|premake) BUILD_SYSTEM=$arg ;;
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
        --bear)        USE_BEAR=true ;;
        --clean)       CLEAN_BUILD=true ;;
    esac
done

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

cd "$PROJECT_ROOT/Build"

if [ "$BUILD_SYSTEM" = "cmake" ]; then
    echo "=== Building project with CMake ($BUILD_CONFIG) ==="

    cd ..

    if [ "$USE_BEAR" = true ]; then
        echo "=== Building with bear (compilation database will be generated) ==="
        bear -- cmake --build Build --config "$BUILD_CONFIG" --parallel
    else
        cmake --build Build --config "$BUILD_CONFIG" --parallel
    fi
else
    echo "=== Building project with Premake/Make ($BUILD_CONFIG) ==="

    MAKE_CONFIG=$(echo "$BUILD_CONFIG" | tr '[:upper:]' '[:lower:]')

    if [ "$CLEAN_BUILD" = true ]; then
        echo "=== Cleaning build ==="
        make clean config=$MAKE_CONFIG
    fi

    if [ "$USE_BEAR" = true ]; then
        echo "=== Building with bear (incremental, compile_commands.json will be generated) ==="
        bear --append -o ../compile_commands.json -- make config=$MAKE_CONFIG
    else
        echo "=== Building project ($MAKE_CONFIG) ==="
        make config=$MAKE_CONFIG
    fi
fi

echo "=== Build complete ==="

# Premake uses lowercase system name (linux-x86_64), CMake uses CMAKE_SYSTEM_NAME (Linux-x86_64)
if [ "$BUILD_SYSTEM" = "cmake" ]; then
    OUTPUT_DIR="Linux-x86_64/$BUILD_CONFIG"
else
    OUTPUT_DIR="linux-x86_64/$BUILD_CONFIG"
fi

EDITOR_DIR="$PROJECT_ROOT/Build/Binaries/$OUTPUT_DIR/Editor"

echo "=== Copying Data and imgui.ini to $EDITOR_DIR ==="
mkdir -p "$EDITOR_DIR"
cp -r "$PROJECT_ROOT/Editor/Data" "$EDITOR_DIR/"
cp "$PROJECT_ROOT/Editor/imgui.ini" "$EDITOR_DIR/"
