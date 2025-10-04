#!/bin/bash

set -e

cd "$(dirname "$0")/.."

echo "=== Cleaning build ==="
make clean

echo "=== Building project (debug) ==="
make config=debug

echo "=== Build complete ==="