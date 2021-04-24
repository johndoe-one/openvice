#include "engine.h"

GLFWwindow* window;

int window_init() {
    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "OpenVice", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (GLEW_OK != err) {
        printf("Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    return 0;
}

void window_loop() {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(255.0, 255.0, 255.0, 255.0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }
}

void window_cleanup() {
    glfwTerminate();
}