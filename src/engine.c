#include "engine.h"

GLFWwindow* window;

int window_init() {
    if (!glfwInit()) {
        printf("Cannot init GLFW library\n");
        return 0;
    }

    window = glfwCreateWindow(640, 480, "OpenVice", NULL, NULL);
    if (!window) {
        printf("Cannot create window in GLFW library\n");
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (GLEW_OK != err) {
        printf("GLFW library error: %s\n", glewGetErrorString(err));
        return 0;
    }

    return 1;
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