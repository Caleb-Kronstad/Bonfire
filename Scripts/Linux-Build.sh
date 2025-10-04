#!/bin/bash

set -e

source "$(dirname "$0")/Linux-Config.sh"

cd "$(dirname "$0")/.."

echo "=== Cleaning build ==="
make clean

echo "=== Building project ($BUILD_CONFIG) ==="
make config=$BUILD_CONFIG

echo "=== Build complete ==="