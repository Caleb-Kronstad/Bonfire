#!/bin/bash

set -e

# Defaults
BUILD_CONFIG=Debug
USE_BEAR=false
CLEAN_BUILD=false

# Parse arguments
for arg in "$@"; do
    case $arg in
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
        --bear)        USE_BEAR=true ;;
        --clean)       CLEAN_BUILD=true ;;
    esac
done

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

cd "$PROJECT_ROOT/Build"

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

echo "=== Build complete ==="

EDITOR_DIR="$PROJECT_ROOT/Build/Binaries/linux-x86_64/$BUILD_CONFIG/Editor"

echo "=== Copying Data to $EDITOR_DIR ==="
mkdir -p "$EDITOR_DIR"
cp -r "$PROJECT_ROOT/Editor/Data" "$EDITOR_DIR/"

if [ ! -f "$EDITOR_DIR/imgui.ini" ]; then
    echo "=== Seeding imgui.ini in $EDITOR_DIR ==="
    cp "$PROJECT_ROOT/Editor/imgui.ini" "$EDITOR_DIR/"
fi
