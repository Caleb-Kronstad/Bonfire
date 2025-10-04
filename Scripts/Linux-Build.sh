#!/bin/bash

set -e

source "$(dirname "$0")/Linux-Config.sh"

cd "$(dirname "$0")/../Build"

# Convert BUILD_CONFIG to lowercase for make
MAKE_CONFIG=$(echo "$BUILD_CONFIG" | tr '[:upper:]' '[:lower:]')

echo "=== Cleaning build ==="
make clean

echo "=== Building project ($MAKE_CONFIG) ==="
make config=$MAKE_CONFIG

echo "=== Build complete ==="
