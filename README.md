# Bonfire Engine
A 3D Graphics Engine

## Prerequisites

### Windows
- Visual Studio 2019+ or compatible C++ compiler
- [Optional] Visual Studio, Rider, or similar IDE

### Linux  
- clang++ or g++
- make
- X11 development libraries: `libx11-dev libxrandr-dev libxi-dev libxxf86vm-dev libxcursor-dev libxinerama-dev`
- [Optional] CLion or similar IDE (install bear for better CLion integration)

## Build Steps

### Windows
1. Run `Scripts/Windows-Setup.bat` (generates project files)
2. Run `Scripts/Windows-Build.bat` 
   - Verify MSBUILD path is correct, or build via IDE
3. Run `Scripts/Windows-Run.bat` or run via IDE

### Linux
1. Run `Scripts/Linux-Setup.sh` (generates makefiles)
2. Run `Scripts/Linux-Build.sh`
3. Run `Scripts/Linux-Run.sh`