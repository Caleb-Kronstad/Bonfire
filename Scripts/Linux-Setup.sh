#!/bin/bash

pushd "$(dirname "$0")/.."

# Defaults
COMPILER=gcc
BUILD_CONFIG=Debug

# Parse arguments
for arg in "$@"; do
    case $arg in
        clang)         COMPILER=clang ;;
        gcc)           COMPILER=gcc ;;
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
    esac
done

echo "=== Setting up Premake build system ==="

if [ "$COMPILER" = "clang" ]; then
    CC_FLAG="--cc=clang"
else
    CC_FLAG=""
fi

Premake/Linux/premake5 $CC_FLAG --file=Build.lua gmake2

echo "=== Premake setup complete ==="
echo "Compiler: $COMPILER"

popd
