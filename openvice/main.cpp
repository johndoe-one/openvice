#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <assert.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <renderware.h>
#include "shader.h"
#include "IMGArchive.h"

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

std::vector<struct Texture> textures;

struct Texture {
	unsigned int textureId;
	std::string textureName;
};

class Mesh {
private:
	unsigned int gVBO, gVAO, gEBO;
	std::vector<float32> vertices;
	std::vector<rw::uint32> indices;
	std::vector<float32> texCoords;

public:
	std::string textureName;
	uint32 faceType;

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

		// vertex attribute
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE, 
			8 * sizeof(rw::float32), 
			(void*)0
		);
		glEnableVertexAttribArray(0);

		// normal attribute
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(rw::float32),
			(void*)(3 * sizeof(rw::float32))
		);
		glEnableVertexAttribArray(1);

		// texture attribute
		glVertexAttribPointer(
			2, 
			2,
			GL_FLOAT, 
			GL_FALSE, 
			8 * sizeof(rw::float32), 
			(void*)(6 * sizeof(rw::float32))
		);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		// cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);
	};

	void drawMesh()
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);

		for (uint32 i = 0; i < textures.size(); i++) {
			if (textures[i].textureName == textureName) {
				unsigned int textureId = textures[i].textureId;
				glBindTexture(GL_TEXTURE_2D, textureId);
				// cout << "draw texture name = " << textureName.c_str() << endl;
			}
		}

		glEnableVertexAttribArray(0); // vertex
		glEnableVertexAttribArray(1); // normal
		glEnableVertexAttribArray(2); // texcoords

		glBindVertexArray(gVAO);

		GLenum face = GL_TRIANGLES;

		if (faceType == rw::FACETYPE_STRIP) {
			face = GL_TRIANGLE_STRIP;
		}
		
		if (faceType == rw::FACETYPE_LIST) {
			face = GL_TRIANGLE_LIST_SUN; // ???
		}

		glDrawElements(face, indices.size(), GL_UNSIGNED_INT, 0);
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

void loadTexture(const char *filename)
{
	ifstream rw(filename, ios::binary);
	rw::TextureDictionary txd;
	txd.read(rw);
	rw.close();

	for (uint32 i = 0; i < txd.texList.size(); i++) {

		rw::NativeTexture& t = txd.texList[i];
		/*cout << i << " " << t.name << " " << t.maskName << " "
			<< " " << t.width[0] << " " << t.height[0] << " "
			<< " " << t.depth << " " << hex << t.rasterFormat << endl;*/

		if (txd.texList[i].platform == rw::PLATFORM_PS2)
			txd.texList[i].convertFromPS2(0x40);

		if (txd.texList[i].platform == rw::PLATFORM_XBOX)
			txd.texList[i].convertFromXbox();

		if (txd.texList[i].dxtCompression)
			txd.texList[i].decompressDxt();

		txd.texList[i].convertTo32Bit();
		txd.texList[i].writeTGA();

		// create opengl texture
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA, // t.hasAlpha ? GL_RGBA : GL_RGB,
			t.width[0],
			t.height[0],
			0,
			GL_RGBA, // t.hasAlpha ? GL_RGBA : GL_RGB, 
			GL_UNSIGNED_BYTE,
			txd.texList[i].texels.front()
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		struct Texture _texture = { texture_id, t.name };
		cout << "Created texture. Name = " << t.name << ". Texture OpenGL ID = " << texture_id << endl;
		textures.push_back(_texture);
	}
}

void cleanupTextures() {
	for (uint32 i = 0; i < textures.size(); i++) {
		glDeleteTextures(1, &textures[i].textureId);
	}
}

IMGArchive* newIMgArchive;

void initImgFile(const char *filepath) {
	newIMgArchive = new IMGArchive(filepath);
}

uchar* getImgFile(uint id) {
	IMGArchiveFile* newFile = newIMgArchive->getFileByID(id);
	if (newFile != NULL)
	{
		//Do some operations
		cout << newFile->fileEntry->fileName << endl;
		//Can get all bytes for the file and write it out into the separate file

		return newFile->fileByteBuffer;
	}

	delete newFile;

	return NULL;
}

void cleanupImgFile() {
	
	delete newIMgArchive;
}

struct Objs {
	unsigned int object_id;
	std::string object_name;
	std::string object_txd_file;
};

std::vector<Objs> objects;

int loadObjsFromFileIde(const char *filepath) {
	char line[80];

	/* Open file */
	FILE* file = fopen(filepath, "r");

	if (!file) {
		printf("Cannot open file %s.\n", filepath);
		return 0;
	}

	int count_objs_items = 0;

	bool is_objs_section = false;

	while (!feof(file)) {
		fgets(line, sizeof(line), file);

		if (strcmp(line, "objs\n") == 0) { // equal
			is_objs_section = true;
			continue;
		}

		if (strcmp(line, "end\n") == 0) {
			is_objs_section = false;
			continue;
		}
		
		if (is_objs_section) {
			count_objs_items++;

			unsigned int object_id;
			std::string object_name;
			std::string object_txd_file;

			int index = 0;

			char delim[] = ", ";
			char* ptr = strtok(line, delim);

			while (ptr != NULL) {
				if (index == 0) {
					object_id = std::stoul(std::string(ptr), nullptr, 0);;
					printf("obj id = %d \n", object_id);
				}

				if (index == 1) {
					object_name = ptr;
					printf("obj name = %s \n", object_name.c_str());
				}

				if (index == 2) {
					object_txd_file = ptr;
					printf("obj txd name = %s \n", object_txd_file.c_str());
				}

				if (index == 2) {
					struct Objs o = { object_id, object_name, object_txd_file };
					objects.push_back(o);
				}

				ptr = strtok(NULL, delim);
				index++;
			}
		}
	}

	fclose(file);
}

class Model {
private:
	std::vector<Mesh*> meshes;

public:
	void createModel(rw::Clump* clump)
	{
		for (uint32 i = 0; i < clump->geometryList.size(); i++) {
			rw::Geometry geometry = clump->geometryList[i];

			Mesh* mesh = new Mesh();

			for (uint32 i = 0; i < geometry.materialList.size(); i++) {
				if (geometry.materialList[i].hasTex) {
					cout << "Model uses texture name = " << geometry.materialList[i].texture.name << endl;
					mesh->textureName = geometry.materialList[i].texture.name;
				}
			}

			for (uint32 i = 0; i < geometry.vertices.size() / 3; i++) {
				rw::float32 x = geometry.vertices[i * 3 + 0];
				rw::float32 y = geometry.vertices[i * 3 + 1];
				rw::float32 z = geometry.vertices[i * 3 + 2];

				mesh->addVertex(x);
				mesh->addVertex(y);
				mesh->addVertex(z);

				// normals
				rw::float32 nx = geometry.vertices[i * 3 + 0];
				rw::float32 ny = geometry.vertices[i * 3 + 1];
				rw::float32 nz = geometry.vertices[i * 3 + 2];

				mesh->addVertex(nx);
				mesh->addVertex(ny);
				mesh->addVertex(nz);

				// textures
				rw::float32 u = geometry.texCoords[0][i * 2 + 0];
				rw::float32 v = geometry.texCoords[0][i * 2 + 1];

				mesh->addVertex(u);
				mesh->addVertex(v);

				// facetype - triangle strip or another
				mesh->faceType = geometry.faceType;
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

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
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


#include <iostream>
#include <istream>
#include <streambuf>
#include <string>

int main(void)
{
	loadObjsFromFileIde("C:\\Games\\Grand Theft Auto Vice City\\data\\maps\\generic.ide");

	initImgFile("C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.img");

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
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
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
	

	// char to ifstream
	uchar *buffer = getImgFile(343); // barrel1

	// load dff from ifstream
	//read_dff_from_istream(buffer, clump);
	 readDFF("C:\\Files\\Projects\\openvice\\barrel1.dff", clump);

	loadTexture("C:\\Files\\Projects\\openvice\\dynbarrels.txd");

	Model *model = new Model();
	model->createModel(clump);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//cout << glGetError();
	assert(glGetError() == GL_NO_ERROR);

	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		processInput(window);

		glUseProgram(programID);

		assert(glGetError() == GL_NO_ERROR);

		glm::mat4 mat_model = glm::mat4(1.0f);
		glm::mat4 mat_view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 MVP = mat_projection * mat_view * mat_model; // Remember, matrix multiplication is the other way around
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		// either set it manually like so:
		glUniform1i(glGetUniformLocation(programID, "texture1"), 0);

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

	cleanupTextures();

	assert(glGetError() == GL_NO_ERROR);

	cleanupImgFile();

	glfwTerminate();

	return 0;
}
