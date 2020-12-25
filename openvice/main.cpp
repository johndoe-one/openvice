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

GLFWwindow* window;

using namespace glm;
using namespace std;

extern int readDFF(const char *, rw::Clump *_clump);

float SCREEN_W = 1920;
float SCREEN_H = 1080;

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

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

struct Geom {
	unsigned int gVAO;
	unsigned int indexCount;
};

class Mesh {
public: 

	unsigned int gVBO, gVAO, gEBO;

	std::vector<struct Geom> geoms;


	void createModel(rw::Clump* clump)
	{

		for (uint32 i = 0; i < clump->geometryList.size(); i++) {
			rw::Geometry geometry = clump->geometryList[i];

			std::vector<rw::float32> ver;
			std::vector<rw::uint32> ind;


			for (uint32 i = 0; i < geometry.vertices.size() / 3; i++) {
				/*cout << ind << vertices[i * 3 + 0] << ", "
					<< vertices[i * 3 + 1] << ", "
					<< vertices[i * 3 + 2] << endl;*/

				rw::float32 x = geometry.vertices[i * 3 + 0],
					y = geometry.vertices[i * 3 + 1],
					z = geometry.vertices[i * 3 + 2];

				cout << x << " " << y << " " << z << endl;

				ver.push_back(x);
				ver.push_back(y);
				ver.push_back(z);
			}

			cout << "count vertices" << ver.size() << endl;

			cout << "index" << endl;

			for (uint32 i = 0; i < geometry.splits.size(); i++) {
				//cout << endl << ind << "Split " << i << " {\n";
				//ind += "  ";
				/*cout << ind << "matIndex: " << splits[i].matIndex << endl;
				cout << ind << "numIndices: " << splits[i].indices.size() << endl;
				cout << ind << "indices {\n";
				if (!detailed)
					cout << ind + "  skipping\n";
				else*/
				for (uint32 j = 0; j < geometry.splits[i].indices.size(); j++) {
					//cout << ind + " " << splits[i].indices[j] << endl;


					cout << geometry.splits[i].indices[j] << endl;

					ind.push_back(geometry.splits[i].indices[j]);
				}

				/*cout << ind << "}\n";
				ind = ind.substr(0, ind.size() - 2);
				cout << ind << "}\n";*/
			}



			glGenVertexArrays(1, &gVAO);
			glGenBuffers(1, &gVBO);
			glGenBuffers(1, &gEBO);

			glBindVertexArray(gVAO);

			glBindBuffer(GL_ARRAY_BUFFER, gVBO);

			glBufferData(GL_ARRAY_BUFFER, ver.size() * sizeof(rw::float32),
				&ver.front(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gEBO);

			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(rw::uint32),
				&ind.front(), GL_STATIC_DRAW);

			glVertexAttribPointer(0,
				3,
				GL_FLOAT,
				GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);

			cout << glGetError();
			assert(glGetError() == GL_NO_ERROR);

			cout << "count index " << ind.size() << endl;

			struct Geom geom = { gVAO, ind.size() };
			geoms.push_back(geom);

		}
	};


	void drawModel() {
		for (uint32 i = 0; i < geoms.size(); i++) {

			glEnableVertexAttribArray(0);
			glBindVertexArray(geoms[i].gVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
			glDrawElements(GL_TRIANGLES, geoms[i].indexCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0); // no need to unbind it every time

			//cout << glGetError();
			assert(glGetError() == GL_NO_ERROR);
		}
	};

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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);


	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}






// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		//getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 



	// Open a window and create its OpenGL context
	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "OpenVice", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		//getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//	// Set the required callback functions
	//glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);



	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	
	

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("C:\\Files\\Projects\\openvice\\openvice\\x64\\Debug\\simple_shader.vs",
		"C:\\Files\\Projects\\openvice\\openvice\\x64\\Debug\\simple_shader.fs");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), SCREEN_W / SCREEN_H, 0.1f, 100.0f);
	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	rw::Clump* clump = new rw::Clump;
	readDFF("C:\\Files\\Projects\\openvice\\openvice\\x64\\Debug\\barrel1.dff", clump);
	
	Mesh* mesh = new Mesh();
	mesh->createModel(clump);
	//createModel(clump);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//cout << glGetError();
	assert(glGetError() == GL_NO_ERROR);
	

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);

		// Use our shader
		glUseProgram(programID);

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		//glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);


		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		

		glm::mat4 MVP = Projection * view * Model; // Remember, matrix multiplication is the other way around

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		//glBindVertexArray(VertexArrayID);
		//// 1rst attribute buffer : vertices
		//glEnableVertexAttribArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		//glVertexAttribPointer(
		//	0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		//	3,                  // size
		//	GL_FLOAT,           // type
		//	GL_FALSE,           // normalized?
		//	0,                  // stride
		//	(void*)0            // array buffer offset
		//);

		//// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

		//glDisableVertexAttribArray(0);

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		//drawModel(clump);
		mesh->drawModel();

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
