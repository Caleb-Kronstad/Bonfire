#!/bin/bash

# Defaults
BUILD_CONFIG=Debug

# Parse arguments
for arg in "$@"; do
    case $arg in
        debug)         BUILD_CONFIG=Debug ;;
        release)       BUILD_CONFIG=Release ;;
        dist)          BUILD_CONFIG=Dist ;;
    esac
done

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

EXECUTABLE="$PROJECT_ROOT/Build/Binaries/linux-x86_64/$BUILD_CONFIG/Editor/Editor"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Executable not found at $EXECUTABLE"
    echo "Please build the project first using Linux-Build.sh"
    exit 1
fi

echo "=== Running Project ($BUILD_CONFIG build) ==="
cd "$(dirname "$EXECUTABLE")"  # Change to executable directory so Assets folder is found
"$EXECUTABLE"
