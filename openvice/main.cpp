#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <renderware.h>
#include "shader.h"

using namespace glm;
using namespace std;

float SCREEN_W = 1920;
float SCREEN_H = 1080;

extern int readDFF(const char *, rw::Clump *_clump);

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCREEN_W / 2.0;
float lastY = SCREEN_H / 2.0;
float fov = 45.0f;

class Mesh {
private:
	unsigned int gVBO, gVAO, gEBO;
	std::vector<float32> vertices;
	std::vector<rw::uint32> indices;

public:
	void addVertex(rw::float32 vertex)
	{
		vertices.push_back(vertex);
	}

	void addIndices(rw::uint32 index)
	{
		indices.push_back(index);
	}

	void createMesh()
	{
		glGenVertexArrays(1, &gVAO);
		glGenBuffers(1, &gVBO);
		glGenBuffers(1, &gEBO);

		glBindVertexArray(gVAO);

		glBindBuffer(GL_ARRAY_BUFFER, gVBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(rw::float32),
			&vertices.front(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(rw::uint32),
			&indices.front(), GL_STATIC_DRAW);

		glVertexAttribPointer(0,
			3,
			GL_FLOAT,
			GL_FALSE, 
			0, 
			(void*)0
		);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		// cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);
	};

	void drawMesh()
	{
		glEnableVertexAttribArray(0);
		glBindVertexArray(gVAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);
	};

	void cleanupMesh()
	{
		glDeleteBuffers(1, &gVBO);
		glDeleteBuffers(1, &gEBO);
		glDeleteVertexArrays(1, &gVAO);

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);
	};
};

class Model {
private:
	std::vector<Mesh*> meshes;

public:
	void createModel(rw::Clump* clump)
	{
		for (uint32 i = 0; i < clump->geometryList.size(); i++) {
			rw::Geometry geometry = clump->geometryList[i];

			std::vector<rw::float32> ver;
			std::vector<rw::uint32> ind;

			Mesh* mesh = new Mesh();

			for (uint32 i = 0; i < geometry.vertices.size() / 3; i++) {
				rw::float32 x = geometry.vertices[i * 3 + 0];
				rw::float32 y = geometry.vertices[i * 3 + 1];
				rw::float32 z = geometry.vertices[i * 3 + 2];

				mesh->addVertex(x);
				mesh->addVertex(y);
				mesh->addVertex(z);
			}

			for (uint32 i = 0; i < geometry.splits.size(); i++) {
				for (uint32 j = 0; j < geometry.splits[i].indices.size(); j++) {
					mesh->addIndices(geometry.splits[i].indices[j]);
				}
			}

			mesh->createMesh();

			meshes.push_back(mesh);
		}
	};

	void drawModel() {
		for (uint32 i = 0; i < meshes.size(); i++) {
			meshes[i]->drawMesh();
		}
	};

	void cleanupModel()
	{
		for (uint32 i = 0; i < meshes.size(); i++) {
			meshes[i]->cleanupMesh();
		}
	}
};

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

int main(void)
{
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 
	
	// Open a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "OpenVice", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		//getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key being pressed below

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("simple_shader.vs",
		"simple_shader.fs");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	glm::mat4 mat_projection = glm::perspective(glm::radians(45.0f), SCREEN_W / SCREEN_H, 0.1f, 100.0f);

	rw::Clump* clump = new rw::Clump;
	readDFF("C:\\Files\\Projects\\openvice\\barrel1.dff", clump);
	
	Model* model = new Model();
	model->createModel(clump);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//cout << glGetError();
	assert(glGetError() == GL_NO_ERROR);
	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		processInput(window);

		glUseProgram(programID);

		assert(glGetError() == GL_NO_ERROR);

		glm::mat4 mat_model = glm::mat4(1.0f);
		glm::mat4 mat_view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 MVP = mat_projection * mat_view * mat_model; // Remember, matrix multiplication is the other way around
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		//drawModel(clump);
		model->drawModel();

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	model->cleanupModel();
	delete model;

	delete clump;
	
	glDeleteProgram(programID);

	glfwTerminate();

	return 0;
}
