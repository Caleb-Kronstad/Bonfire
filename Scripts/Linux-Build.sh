#!/bin/bash

set -e

# Defaults
BUILD_SYSTEM=premake
BUILD_CONFIG=Debug
USE_BEAR=false

# Parse arguments
for arg in "$@"; do
    case $arg in
        cmake|premake) BUILD_SYSTEM=$arg ;;
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
        --bear)        USE_BEAR=true ;;
    esac
done

cd "$(dirname "$0")/../Build"

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

    if [ "$USE_BEAR" = true ]; then
        echo "=== Building with bear (incremental, compile_commands.json will be generated) ==="
        bear -o ../compile_commands.json -- make config=$MAKE_CONFIG
    else
        echo "=== Cleaning build ==="
        make clean

        echo "=== Building project ($MAKE_CONFIG) ==="
        make config=$MAKE_CONFIG
    fi
fi

echo "=== Build complete ==="
