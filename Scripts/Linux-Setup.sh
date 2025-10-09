#!/bin/bash

source "$(dirname "$0")/Linux-Config.sh"

pushd ..

if [ "$COMPILER" = "clang" ]; then
    CC_FLAG="--cc=clang"
else
    CC_FLAG=""
fi

Premake/Linux/premake5 $CC_FLAG --file=Build.lua gmake2

popd
