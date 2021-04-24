#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 texCoord;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

out vec2 TexCoord;
out vec3 Normal;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	TexCoord = texCoord;
	Normal = aNormal;
}
