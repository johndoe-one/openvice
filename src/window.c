#include "window.h"

static GLFWwindow* window;

static void key_callback(GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods)
{
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
}

int window_init()
{
        GLenum err;

        if (!glfwInit()) {
                printf("Cannot init GLFW library\n");
                return 1;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 
                NULL, NULL);

        if (!window) {
                printf("Cannot create window in GLFW library\n");
                return 1;
        }

        glfwSetKeyCallback(window, key_callback);
        glfwMakeContextCurrent(window);

        err = glewInit();

        if (GLEW_OK != err) {
                printf("GLFW library error: %s\n", glewGetErrorString(err));
                return 1;
        }

        return 0;
}

void window_loop()
{
        while (!glfwWindowShouldClose(window)) {
                glClear(GL_COLOR_BUFFER_BIT);
                glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

                glfwSwapBuffers(window);

                glfwPollEvents();
        }
}

void window_cleanup()
{
        if (window) {
                glfwDestroyWindow(window);
        }

        glfwTerminate();
}
