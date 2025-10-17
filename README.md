# Bonfire

  A cross-platform 3D game engine built with modern C++ and OpenGL, featuring an entity-component system,
  ImGui-based editor interface, and support for 3D model rendering with shaders, textures, and camera controls.

## Build Prerequisites

### Windows

  - Visual Studio 2019+ or compatible C++ compiler
  - [Optional] Visual Studio, Rider, or similar IDE

### Linux

  - clang++ or g++
  - make or CMake
  - X11 development libraries: libx11-dev libxrandr-dev libxi-dev libxxf86vm-dev libxcursor-dev libxinerama-dev

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
  (Community/Professional/Enterprise), edit ```Scripts/Windows-Config.bat``` and update the MSBUILD path.  
    - Alternatively, open Bonfire.sln and build via your IDE (RECOMMENDED)  
  3. Run ```Scripts/Windows-Run.bat```  
    - Alternatively, run via your IDE
    - Executable location: ```Build/Binaries/windows-x86_64/Debug/Project/Project.exe``` 
  
  IMPORTANT: If you are building and running via IDE and do not want to setup custom run configurations you will need to follow the instructions for commenting/uncommenting post-build commands in ```Project/Build-Project.lua```

### Linux

  1. Run ```Scripts/Linux-Setup.sh``` to generate makefiles using Premake5 (change compiler in ```Scripts/Linux-Config.sh```)  
  2. Run ```Scripts/Linux-Build.sh``` to build selected configuration (change configuration in ```Scripts/Linux-Config.sh```)  
  3. Run ```Scripts/Linux-Run.sh```  
    - Executable location: ```Build/Binaries/linux-x86_64/Debug/Project/Project```

  IMPORTANT: If you plan to build via CMake (recommended if using an IDE like CLion) you will need to use the ```cmake``` flag when running all 3 shell scripts listed above. Premake is the default but you can also specify using the ```premake``` flag
  * If using Premake (default):
    - ./Linux-Setup premake
    - ./Linux-Build premake
    - ./Linux-Run premake
  * If using CMake: 
    - ./Linux-Setup cmake  
    - ./Linux-Build cmake
    - ./Linux-Run cmake

## Build System

  Bonfire uses Premake5 and CMake for cross-platform project generation  
  Currently only the Debug config works properly, though you are welcome to try Dist and Release

<p align="center">
  <img src="Docs/bonfire-animation.gif" alt="Bonfire Engine Demo">
</p>
