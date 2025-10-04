#!/bin/bash

set -e

cd "$(dirname "$0")/.."

echo "=== Running project ==="
cd bin/linux-x86_64/Debug/Project
./Project
