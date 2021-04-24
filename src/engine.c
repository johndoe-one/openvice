#include "engine.h"

GLFWwindow* window;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int window_init() {
    if (!glfwInit()) {
        printf("Cannot init GLFW library\n");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(640, 480, "OpenVice", NULL, NULL);
    if (!window) {
        printf("Cannot create window in GLFW library\n");
        return 0;
    }

    glfwSetKeyCallback(window, key_callback);
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
    if (window) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
}