# OpenVice
OpenVice - open source cross-platform engine for Grand Theft Auto: Vice City. Code written on C language for simple and clear development (C language with K&R / Linux Kernel code style). You will need an original copy of the Grand Theft Auto: Vice City to using OpenVice.

## System requirments 
* Supported OS: Windows, GNU/Linux, macOS;
* OpenGL 3.3.

## Build
Project uses C language and cross-platform libraries, so you can build engine for many OS (including Windows, GNU/Linux, macOS).

### Windows
1. Install **Visual Studio Community 2019** (or any Visual Studio version);
2. Copy GLFW source code to **glfw_source/** folder in root folder project;
3. Copy GLEW source code to **glew_source/** folder in root folder project;
3. Copy CGLM source code to **cglm_source/** folder in root folder project;
4. Open **Command shell**;
5. Go to project root directory;
6. Build project with **cmake** (or with cmake-gui);
7. Execute **openvice.exe**

### Linux
1. Install **GCC**, **cmake** (sudo apt-get install gcc cmake);
2. Copy GLFW source code to **glfw_source/** folder in root folder project;
3. Copy GLEW source code to **glew_source/** folder in root folder project;
3. Copy CGLM source code to **cglm_source/** folder in root folder project;
4. Open **Terminal**;
5. Go to project root directory;
6. Build project with **cmake**;
7. Execute **./openvice**

### Dependencies
* [CGLM](https://github.com/recp/cglm) - Mathematics library (with OpenGL compatibility)
* [GLFW](https://www.glfw.org/) - Creating windows, contexts and surfaces, receiving input and events;
* [GLEW](http://glew.sourceforge.net/) - OpenGL Extension Wrangler Library;
* [CMake](https://cmake.org/) - Cross-platform tool for build, test, package software.

## License
[GNU GPL 2](https://en.wikipedia.org/wiki/GNU_General_Public_License#Version_2).

## Links
* [Grand Theft Auto: Vice City](https://www.rockstargames.com/games/vicecity).
