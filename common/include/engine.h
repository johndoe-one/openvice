#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h> // printf

int window_init();
void window_loop();
void window_cleanup(); 

#endif