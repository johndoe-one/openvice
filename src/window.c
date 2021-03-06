/*
 * Copyright (C) 2021 Misha Samoylov
 */

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

        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                camera_forward();
        }

        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
                camera_backward();
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

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        return 0;
}

void window_loop(struct model model)
{
        while (!glfwWindowShouldClose(window)) {
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                model_draw(model);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }
}

void window_cleanup()
{
        if (window)
                glfwDestroyWindow(window);

        glfwTerminate();
}

static int create_program_shader(GLuint *pshader_program)
{
        int success;
        char info_log[512];

        unsigned int vertex_shader;
        unsigned int fragment_shader;
        unsigned int shader_program;

        const char* vertex_shader_source = "#version 330 core\n"
                "layout (location = 0) in vec3 aPos;\n"
                "uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "void main()\n"
                "{\n"
                "    gl_Position = projection * view * model * vec4(aPos, 1.0);"
                "\n"
                "}\0";
        const char* fragment_shader_source = "#version 330 core\n"
                "out vec4 FragColor;\n"
                "void main()\n"
                "{\n"
                "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
                "}\0";

        /* vertex shader */
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
        glCompileShader(vertex_shader);

        /* check for shader compile errors */
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
                printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n",
                        info_log);

                return 1;
        }

        /* fragment shader */
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
        glCompileShader(fragment_shader);

        /* check for shader compile errors */
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
                glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
                printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED %s\n",
                        info_log);

                return 1;
        }

        /* link shaders */
        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        /* check for linking errors */
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
                glGetProgramInfoLog(shader_program, 512, NULL, info_log);
                printf("ERROR::SHADER::PROGRAM::LINKING_FAILED %s\n", info_log);

                return 1;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        *pshader_program = shader_program;

        return 0;
}

int model_init(struct model *pmodel)
{
        GLuint shader_program;
        struct model model;
        int err;

        err = create_program_shader(&shader_program);
        if (err) {
                printf("Cannot create program shader\n");
                return 1;
        }

        float vertices[] = {
                0.5f,  0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                -0.5f, -0.5f, 0.0f,
                -0.5f,  0.5f, 0.0f
        };
        unsigned int indices[] = {
                0, 1, 3,
                1, 2, 3
        };

        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, 
                GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);

        model.VAO = VAO;
        model.VBO = VBO;
        model.EBO = EBO;
        model.shader_program = shader_program;

        *pmodel = model;

        return 0;
}

void model_draw(struct model model)
{
        mat4 m_projection, m_view, m_model;
        GLuint projection_location, view_location, model_location;

        camera_get_projection(m_projection);
        camera_get_view(m_view);
        glm_mat4_identity(m_model);

        glUseProgram(model.shader_program);

        projection_location = glGetUniformLocation(model.shader_program,
                "projection");
        view_location = glGetUniformLocation(model.shader_program, "view");
        model_location = glGetUniformLocation(model.shader_program, "model");

        glUniformMatrix4fv(projection_location, 1, GL_FALSE, m_projection[0]);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, m_view[0]);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, m_model[0]);

        glBindVertexArray(model.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void model_cleanup(struct model model)
{
        glDeleteVertexArrays(1, &model.VAO);
        glDeleteBuffers(1, &model.VBO);
        glDeleteBuffers(1, &model.EBO);

        glDeleteProgram(model.shader_program);
}
