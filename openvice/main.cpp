#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>
#include <fstream>
#include <istream>
#include <streambuf>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "rwtools-master/renderware.h"
#include "shader.h"
#include "loader_img.h"
#include "camera.h"

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

extern int readDFF(std::istream& in, rw::Clump* _clump);

Camera camera(cameraPos, cameraUp, yaw, pitch);

struct Texture {
	unsigned int textureOpenGLId;
	std::string textureName;
};

struct _Material {
	int materialIndex;
	std::string textureName;
};

std::vector<struct Texture> all_textures;

class Mesh {
private:
	unsigned int gVBO, gVAO, gEBO;	
	std::vector<rw::uint32> indices;

public:
	//uint32_t material_id;
	std::vector<rw::float32> vertices;
	std::vector<rw::float32> texCoords;

	std::string textureName;
	rw::uint32 faceType;

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

		//for (uint32 i = 0; i < all_textures.size(); i++) {
		//	if (all_textures[i].textureName == materials[material_id].textureName) {

		//		glBindTexture(GL_TEXTURE_2D, all_textures[i].textureOpenGLId);

		//		unsigned int textureId = all_textures[i].textureOpenGLId;
		//		glBindTexture(GL_TEXTURE_2D, textureId);
		//		// cout << "draw texture name = " << textureName.c_str() << endl;
		//		break;
		//	}
		//}
		

		for (rw::uint32 i = 0; i < all_textures.size(); i++) {
			if (all_textures[i].textureName == textureName) {
				unsigned int textureId = all_textures[i].textureOpenGLId;
				glBindTexture(GL_TEXTURE_2D, textureId);
				//cout << "draw texture name = " << textureName.c_str() << endl;
				break;
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

		//glBindTexture(GL_TEXTURE_2D, 0);

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


void loadTextureFromStream(std::istream &rw)
{
	//ifstream rw(filename, ios::binary);
	rw::TextureDictionary txd;
	txd.read(rw);
	//rw.close();

	for (rw::uint32 i = 0; i < txd.texList.size(); i++) {

		rw::NativeTexture& t = txd.texList[i];
		/*cout << i << " " << t.name << " " << t.maskName << " "
			<< " " << t.width[0] << " " << t.height[0] << " "
			<< " " << t.depth << " " << hex << t.rasterFormat << endl;*/

		/*if (txd.texList[i].platform == rw::PLATFORM_PS2)
			txd.texList[i].convertFromPS2(0x40);

		if (txd.texList[i].platform == rw::PLATFORM_XBOX)
			txd.texList[i].convertFromXbox();*/

		if (txd.texList[i].dxtCompression)
			txd.texList[i].decompressDxt();

		txd.texList[i].convertTo32Bit();
		//txd.texList[i].writeTGA();

		//if (t.width[0] % 2 == 0 || t.height[0] == 0) { // not square image
		//	continue;
		//}

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

		// cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		struct Texture _texture = { texture_id, t.name };
		std::cout << "Created texture. Name = " << t.name << ". Texture OpenGL ID = " << texture_id << std::endl;
		all_textures.push_back(_texture);
	}
}

void cleanupTextures() {
	for (rw::uint32 i = 0; i < all_textures.size(); i++) {
		glDeleteTextures(1, &all_textures[i].textureOpenGLId);
	}
}

class Model {
private:
	std::vector<Mesh*> meshes;
	std::vector<_Material> materials;
	std::vector<rw::float32> vert;

public:
	void createModel(rw::Clump* clump)
	{
		for (rw::uint32 i = 0; i < clump->geometryList.size(); i++) {
			rw::Geometry geometry = clump->geometryList[i];

			for (rw::uint32 i = 0; i < geometry.materialList.size(); i++) {
				if (geometry.materialList[i].hasTex) {
					std::cout << "Model uses material id: " << i <<
						" with texture_name = " << geometry.materialList[i].texture.name << std::endl;

					//mesh->textureName = geometry.materialList[i].texture.name;

					struct _Material mat = { i, geometry.materialList[i].texture.name };
					materials.push_back(mat);
				}
			}
		}


		for (rw::uint32 i = 0; i < clump->geometryList.size(); i++) {
			rw::Geometry geometry = clump->geometryList[i];

			

			//for (rw::uint32 i = 0; i < geometry.materialList.size(); i++) {
			//	if (geometry.materialList[i].hasTex) {
			//		std::cout << "Model uses material id: " << i << 
			//			" with texture_name = " << geometry.materialList[i].texture.name << std::endl;

			//		//mesh->textureName = geometry.materialList[i].texture.name;

			//		struct _Material mat = { i, geometry.materialList[i].texture.name };
			//		materials.push_back(mat);
			//	}
			//}

			for (rw::uint32 i = 0; i < geometry.vertices.size() / 3; i++) {
				// vertices
				rw::float32 x = geometry.vertices[i * 3 + 0];
				rw::float32 y = geometry.vertices[i * 3 + 1];
				rw::float32 z = geometry.vertices[i * 3 + 2];

				// fix Y is cameraUP
				rw::float32 temp = y;
				y = z;
				z = temp;

				vert.push_back(x);
				vert.push_back(y);
				vert.push_back(z);

				// normals
				rw::float32 nx = geometry.vertices[i * 3 + 0];
				rw::float32 ny = geometry.vertices[i * 3 + 1];
				rw::float32 nz = geometry.vertices[i * 3 + 2];

				vert.push_back(nx);
				vert.push_back(ny);
				vert.push_back(nz);

				// texture coordinates
				rw::float32 u = geometry.texCoords[0][i * 2 + 0];
				rw::float32 v = geometry.texCoords[0][i * 2 + 1];

				vert.push_back(1.0 - u); // fix Y is cameraUP
				vert.push_back(v);
			}

			for (rw::uint32 i = 0; i < geometry.splits.size(); i++) {

				Mesh* mesh = new Mesh();
				mesh->faceType = geometry.faceType; // facetype - triangle strip or another
				mesh->vertices = vert;

				rw::uint32 mat_ind = geometry.splits[i].matIndex;
				
				if (mat_ind < materials.size()) { // check Array index out of bounds
					mesh->textureName = materials[mat_ind].textureName;
				}

				for (rw::uint32 j = 0; j < geometry.splits[i].indices.size(); j++) {
					mesh->addIndices(geometry.splits[i].indices[j]);

					//mesh->material_id = geometry.splits[i].matIndex;
					std::cout << "indices uses texture = " << mesh->textureName << std::endl;
				}

				mesh->createMesh();
				meshes.push_back(mesh);
			}
		}
	};

	void drawModel() {
		for (rw::uint32 i = 0; i < meshes.size(); i++) {
			meshes[i]->drawMesh();
		}
	};

	void cleanupModel()
	{
		for (rw::uint32 i = 0; i < meshes.size(); i++) {
			meshes[i]->cleanupMesh();
		}
	}
};

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}


LoaderIMG* loaderImg = new LoaderIMG();

class RenderModel {
public:
	rw::Clump* clump;
	Model* model;

	int model_id;

	bool initModel(int _model_id, const char *dff_name, const char *txd_name) 
	{
		model_id = _model_id;

		std::string assetNameInFileImg(dff_name);
		assetNameInFileImg += ".dff";

		std::string assetTextureFromArchive(txd_name);
		assetTextureFromArchive += ".txd";

		auto tex_raw_data = loaderImg->loadToMemory(assetTextureFromArchive);
		if (!tex_raw_data) {
			return false;
		}

		LoaderIMGFile asset_tex;
		if (!loaderImg->findAssetInfo(assetTextureFromArchive, asset_tex)) {
			return false;
		}

		constexpr size_t kAssetRecordSize{ 2048 };

		std::stringstream texStream;
		texStream.write(tex_raw_data.get(), kAssetRecordSize * asset_tex.size);

		loadTextureFromStream(texStream);

		// write to IN file info about model
		auto raw_data = loaderImg->loadToMemory(assetNameInFileImg);
		if (!raw_data) {
			return false;
		}

		LoaderIMGFile asset;
		if (!loaderImg->findAssetInfo(assetNameInFileImg, asset)) {
			return false;
		}

		std::stringstream stream;
		stream.write(raw_data.get(), kAssetRecordSize * asset.size);

		//
		clump = new rw::Clump();
		readDFF(stream, clump);

		model = new Model();
		model->createModel(clump);

		return true;
	}

	void draw() {
		model->drawModel();
	}

	void cleanup() {
		model->cleanupModel();
		clump->clear();

		delete model;
		delete clump;
	}
};

std::vector<RenderModel*> all_models;

struct ide_file_object {
	int object_id;
	std::string dff_name; // wo extension
	std::string txd_name; // wo extension
};

std::vector<ide_file_object> ide_file_objects;

int load_file_ide(const char* filepath) {
	char line[128];

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
					// printf("IDE file. Obj id = %d \n", object_id);
				}

				if (index == 1) {
					object_name = ptr;
					// printf("IDE file. Obj name = %s \n", object_name.c_str());
				}

				if (index == 2) {
					object_txd_file = ptr;
					// printf("IDE file. Obj txd name = %s \n", object_txd_file.c_str());
				}

				if (index == 2) {
					struct ide_file_object obj = { object_id, object_name, object_txd_file };
					ide_file_objects.push_back(obj);

					printf("IDE obj id = %d dff_name = %s txd_name = %s \n", object_id, object_name.c_str(), object_txd_file.c_str());
				}

				ptr = strtok(NULL, delim);
				index++;
			}
		}
	}

	fclose(file);
}

void load_resources() {

	// load models
	for (int i = 0; i < ide_file_objects.size(); i++) {
		struct ide_file_object obj;
		obj = ide_file_objects[i];

		// load to all_models
		RenderModel* rm = new RenderModel();

		bool success_loaded_model = rm->initModel(obj.object_id, obj.dff_name.c_str(), obj.txd_name.c_str());

		if (success_loaded_model) {
			all_models.push_back(rm);
		}
	}
}

struct ipl_file_object {
	int obj_id;
	float x;
	float y;
	float z;
};

std::vector<ipl_file_object> ipl_file_objects;

// placement objects
int load_file_ipl(const char* filepath) {
	char line[256];

	FILE* file = fopen(filepath, "r");

	if (!file) {
		printf("Cannot open file %s.\n", filepath);
		return 0;
	}

	int count_objs_items = 0;

	bool is_objs_section = false;

	while (!feof(file)) {
		fgets(line, sizeof(line), file);

		if (strcmp(line, "inst\n") == 0) { // equal
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
			float x,y,z;

			int index = 0;

			char delim[] = ", ";
			char* ptr = strtok(line, delim);

			while (ptr != NULL) {
				if (index == 0) {
					object_id = std::stoul(std::string(ptr), nullptr, 0);
					//printf("IPL obj id = %d \n", object_id);
				}

				if (index == 1) {
					object_name = ptr;
					//printf("IPL obj name = %s \n", object_name.c_str());
				}

				if (index == 3) { // x
					x = atof(ptr);
					//printf("IPL obj X = %f \n", x);
				}

				if (index == 4) { // y
					y = atof(ptr);
					//printf("IPL obj Y = %f \n", y);
				}

				if (index == 5) { // z
					z = atof(ptr);

					printf("IPL obj id = %d x = %f y = %f z = %f \n", object_id, x, y, z);

					// fix Y is UP
					float temp = y;
					y = z;
					z = temp;

					struct ipl_file_object obj = { object_id, x,y,z };
					ipl_file_objects.push_back(obj);
				}

				ptr = strtok(NULL, delim);
				index++;
			}
		}
	}

	fclose(file);
}

void cleanupRenderModels() {
	for (rw::uint32 i = 0; i < all_models.size(); i++) {
		all_models[i]->cleanup();
	}
}

int main(void)
{
	load_file_ipl("C:\\Games\\Grand Theft Auto Vice City\\data\\maps\\starisl\\starisl.ipl");
	load_file_ide("C:\\Games\\Grand Theft Auto Vice City\\data\\maps\\starisl\\starisl.ide");

	loaderImg->load("C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.dir",
		"C:\\Games\\Grand Theft Auto Vice City\\models\\gta3.img");

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

	glm::mat4 mat_projection = glm::perspective(glm::radians(45.0f), SCREEN_W / SCREEN_H, 0.1f, 999999.0f);


	load_resources();

	assert(glGetError() == GL_NO_ERROR);

	glEnable(GL_DEPTH_TEST);
	
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		processInput(window);

		glUseProgram(programID);

		assert(glGetError() == GL_NO_ERROR);

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		// draw all models
		for (size_t i = 0; i < ipl_file_objects.size(); i++) {
			struct ipl_file_object wo = ipl_file_objects[i];

			for (size_t j = 0; j < all_models.size(); j++) {
				if (wo.obj_id == all_models[j]->model_id) {

					glm::mat4 mat_model = glm::mat4(1.0f);
					mat_model = glm::translate(mat_model, glm::vec3(wo.x, wo.y, wo.z));

					glm::mat4 mat_view = camera.GetViewMatrix();
					glm::mat4 MVP = mat_projection * mat_view * mat_model; // Remember, matrix multiplication is the other way around

					glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

					// either set it manually like so:
					glUniform1i(glGetUniformLocation(programID, "texture1"), 0);

					all_models[j]->draw();
				}
			}
		}

		//cout << glGetError();
		assert(glGetError() == GL_NO_ERROR);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanupRenderModels();
	
	glDeleteProgram(programID);

	cleanupTextures();
	assert(glGetError() == GL_NO_ERROR);
	
	// TODO cleanup loaderImg

	glfwTerminate();

	return 0;
}
