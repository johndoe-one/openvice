# OpenVice
OpenVice - open source engine for Grand Theft Auto: Vice City. Engine written on C language for simple and clear development (standard C language C89 with K&R / Linux Kernel code style). You will need an original copy of the Grand Theft Auto: Vice City to use this engine.

## System requirments 
* Supported OS: Windows, GNU/Linux, macOS;
* OpenGL 3.3.

## Build
Project uses C89 language and cross-platform libraries so you can build engine for many OS (including Windows, GNU/Linux, macOS).

### Windows
1. Install [MinGW](https://code.visualstudio.com/docs/languages/cpp#_example-install-mingwx64);
1.1. Build from source libraries: GLFW, GLEW;
2. Open Command Shell;
3. Go to source directory;
4. Enter **mingw32-make.exe**;
5. In folder **build** execute app **openvice.exe**.

### Linux
1. Install GCC, GLFW, GLEW, Make (sudo apt-get install gcc glfw glew make);
2. Open Terminal;
3. Go to source directory;
4. Enter **make**;
5. In folder **build** execute app **openvice**.

### Dependencies
* [GLFW](https://www.glfw.org/) - Creating windows, contexts and surfaces, receiving input and events;
* [GLEW](http://glew.sourceforge.net/) - OpenGL Extension Wrangler Library.

## License
[GNU GPL 2](https://en.wikipedia.org/wiki/GNU_General_Public_License#Version_2)

## Links
* [Grand Theft Auto: Vice City](https://www.rockstargames.com/games/vicecity)