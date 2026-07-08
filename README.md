# Bonfire

  A cross-platform 3D game engine built with C++ and OpenGL.

## Build

Supports Windows and Linux, with either Premake or CMake as the build system.

#### 1. Clone the repository with submodules
```
git clone --recursive https://github.com/Caleb-Kronstad/Bonfire Bonfire
cd Bonfire
```
Note: The --recursive flag automatically downloads all dependencies (~200MB)

#### 2. Run the setup and build scripts

**Windows**
```
cd Scripts
Windows-Setup.bat [premake|cmake]
Windows-Build.bat
Windows-Run.bat
```
Note: Generates a Visual Studio 2022 solution in the Build directory. Build settings (config, MSBuild path) are configured in `Windows-Config.bat`.

**Linux**
```
cd Scripts
./Linux-Setup.sh [premake|cmake] [gcc|clang] [debug|release|dist]
./Linux-Build.sh [premake|cmake] [debug|release|dist]
./Linux-Run.sh [premake|cmake] [debug|release|dist]
```

`Linux-Build.sh` builds incrementally by default; pass `--clean` to force a full rebuild of the selected config.

See `Scripts/INSTRUCTIONS.txt` for the full set of options, including generating a `compile_commands.json` for CLion via `./Linux-Build.sh --bear`.

More information can be found at [bonfireengine.com](https://bonfireengine.com/download)

<p align="center">
  <img src="Docs/bonfire-animation.gif" alt="Bonfire Engine Demo">
</p>
