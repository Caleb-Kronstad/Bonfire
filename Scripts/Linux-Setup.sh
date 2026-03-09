#!/bin/bash

pushd "$(dirname "$0")/.."

# Defaults
BUILD_SYSTEM=premake
COMPILER=gcc
BUILD_CONFIG=Debug

# Parse arguments
for arg in "$@"; do
    case $arg in
        cmake|premake) BUILD_SYSTEM=$arg ;;
        clang)         COMPILER=clang ;;
        gcc)           COMPILER=gcc ;;
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
    esac
done

if [ "$BUILD_SYSTEM" = "cmake" ]; then
    echo "=== Setting up CMake build system ==="

    if [ "$COMPILER" = "clang" ]; then
        export CC=clang
        export CXX=clang++
    else
        export CC=gcc
        export CXX=g++
    fi

    cmake -S . -B Build \
        -DCMAKE_BUILD_TYPE="$BUILD_CONFIG" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -G "Unix Makefiles"

    echo "=== CMake setup complete ==="
    echo "Build type: $BUILD_CONFIG"
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
    echo "Compiler: $COMPILER"
fi

popd
