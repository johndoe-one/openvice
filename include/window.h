/*
 * Create window, OpengGL context, receive events, main loop
 * Copyright (C) 2021 Misha Samoylov
 */

#ifndef ENGINE_H
#define ENGINE_H

#define WINDOW_TITLE "OpenVice"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#include <stdio.h> /* printf */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int window_init();
void window_loop();
void window_cleanup(); 

#endif