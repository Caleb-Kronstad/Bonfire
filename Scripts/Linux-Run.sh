#!/bin/bash

# Exit on any error
set -e

# Go to root Bonfire directory
cd "$(dirname "$0")/.."

echo "=== Running project ==="
cd bin/linux-x86_64/Debug/Project
./Project
