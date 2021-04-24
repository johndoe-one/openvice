# Windows: install MinGW for compile
# https://code.visualstudio.com/docs/languages/cpp#_example-install-mingwx64

# Windows: terminal command to build project
# mingw32-male.exe

INCLUDE_APP = ./include

INCLUDE_GLFW = C:\Users\Misha\Downloads\glfw-3.3.4.bin.WIN64\include
INCLUDE_GLEW = C:\msys64\home\Misha\glew-2.1.0\include

LIB_GLFW = C:\Users\Misha\Downloads\glfw-3.3.4.bin.WIN64\lib-mingw-w64
LIB_GLEW = C:\msys64\home\Misha\glew-2.1.0\lib

all:
	gcc -g -Wall ./src/main.c ./src/file_img.c ./src/file_dir.c ./src/engine.c -I $(INCLUDE_APP) -I $(INCLUDE_GLFW) -I $(INCLUDE_GLEW) -L $(LIB_GLFW) -l glfw3dll -L $(LIB_GLEW) -l glew32.dll -l opengl32 -o ./build/openvice
