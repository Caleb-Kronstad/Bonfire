#!/bin/bash

source "$(dirname "$0")/Linux-Config.sh"

pushd "$(dirname "$0")/.."

# Parse build system argument (default to premake)
BUILD_SYSTEM=${1:-premake}

if [ "$BUILD_SYSTEM" = "cmake" ]; then
    echo "=== Setting up CMake build system ==="

    # Determine compiler
    if [ "$COMPILER" = "clang" ]; then
        export CC=clang
        export CXX=clang++
    else
        export CC=gcc
        export CXX=g++
    fi

    CMAKE_BUILD_TYPE="$BUILD_CONFIG"

    # Generate build files
    cmake -S . -B Build \
        -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
        -G "Unix Makefiles"

    echo "=== CMake setup complete ==="
    echo "Build type: $CMAKE_BUILD_TYPE"
    echo "Compiler: $COMPILER"
else
    echo "=== Setting up Premake build system ==="

    if [ "$COMPILER" = "clang" ]; then
        CC_FLAG="--cc=clang"
    else
        CC_FLAG=""
    fi

    Premake/Linux/premake5 $CC_FLAG --file=Build.lua gmake2

    echo "=== Premake setup complete ==="
fi

popd
