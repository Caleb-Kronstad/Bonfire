#!/bin/bash

set -e

source "$(dirname "$0")/Linux-Config.sh"

cd "$(dirname "$0")/.."

echo "=== Running project ==="
cd Build/Binaries/linux-x86_64/$BUILD_CONFIG/Project
./Project