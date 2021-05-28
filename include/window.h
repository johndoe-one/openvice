/*
 * Create window, OpengGL context, receive events, main loop
 * Copyright (C) 2021 Misha Samoylov
 */

#ifndef ENGINE_H
#define ENGINE_H

#define WINDOW_TITLE "OpenVice"
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"

struct model {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        GLuint shader_program;
};

int window_init();
void window_loop(struct model model);
void window_cleanup();

int model_init(struct model *model);
void model_draw(struct model model);
void model_cleanup(struct model model);

#endif