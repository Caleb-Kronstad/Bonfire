#!/bin/bash

# Exit on any error
set -e

# Go to root Bonfire directory
cd "$(dirname "$0")/.."

echo "=== Cleaning build ==="
make clean

echo "=== Building project (debug) ==="
make config=debug

echo "=== Build complete ==="
