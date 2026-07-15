#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

"$SCRIPT_DIR/Linux-Build.sh" "$@"
"$SCRIPT_DIR/Linux-Run.sh" "$@"
