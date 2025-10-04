# Bonfire Engine

## Build Prerequisites

### Windows

  - Visual Studio 2019+ or compatible C++ compiler
  - [Optional] Visual Studio, Rider, or similar IDE

### Linux

  - clang++ or g++
  - make
  - X11 development libraries: libx11-dev libxrandr-dev libxi-dev libxxf86vm-dev libxcursor-dev libxinerama-dev
  - [Optional] CLion or similar IDE (install bear for better CLion integration)

## Build Steps

### Windows

  1. Run Scripts/Windows-Setup.bat (generates Visual Studio solution using Premake5)
  2. Run Scripts/Windows-Build.bat to build Debug configuration
    - Note: The script uses MSBuild from Visual Studio 2022 Preview. If you have a different edition
  (Community/Professional/Enterprise), edit Scripts/Windows-Build.bat and update the MSBUILD path (line 5)
    - Alternatively, open Bonfire.sln and build via your IDE
  3. Run Scripts/Windows-Run.bat or run via IDE
    - Executable location: bin/windows-x86_64/Debug/Project/Project.exe

### Linux

  1. Run Scripts/Linux-Setup.sh (generates makefiles using Premake5)
    - Note: By default, this uses clang. To use g++ instead, edit Scripts/Linux-Setup.sh and remove --cc=clang
  2. Run Scripts/Linux-Build.sh to build Debug configuration
  3. Run Scripts/Linux-Run.sh
    - Executable location: bin/linux-x86_64/Debug/Project/Project

## Build System

  Bonfire uses Premake5 for cross-platform project generation.
  Currently only the Debug build works properly -- Release and Dist will be implemented at a later date