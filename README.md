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

## Build Instructions

Clone the repository with submodules  
  ```bash
  git clone --recursive https://github.com/Caleb-Kronstad/Bonfire Bonfire  
  cd Bonfire  
  ```
  Note: The --recursive flag automatically downloads all dependencies (~200MB)

### Windows

  1. Run ```Scripts/Windows-Setup.bat``` to generate Visual Studio solution using Premake5
  2. Run ```Scripts/Windows-Build.bat``` to build selected configuration (change configuration in ```Scripts/Windows-Config.bat```)  
    - Note: The script uses MSBuild from Visual Studio 2022 Preview. If you have a different edition
  (Community/Professional/Enterprise), edit ```Scripts/Windows-Config.bat``` and update the MSBUILD path (line 5)  
    - Alternatively, open Bonfire.sln and build via your IDE
  3. Run ```Scripts/Windows-Run.bat``` or run via IDE  
    - Executable location: ```Build/Binaries/windows-x86_64/Debug/Project/Project.exe```

### Linux

  1. Run ```Scripts/Linux-Setup.sh``` to generate makefiles using Premake5 (change compiler in ```Scripts/Linux-Config.sh```)  
  2. Run ```Scripts/Linux-Build.sh``` to build selected configuration (change configuration in ```Scripts/Linux-Config.sh```)  
  3. Run ```Scripts/Linux-Run.sh```  
    - Executable location: ```Build/Binaries/linux-x86_64/Debug/Project/Project```

## Build System

  Bonfire uses Premake5 for cross-platform project generation.  
  Currently only the Debug config works properly

<p align="center">
  <img src="Docs/bonfire-animation.gif" alt="Bonfire Engine Demo">
</p>