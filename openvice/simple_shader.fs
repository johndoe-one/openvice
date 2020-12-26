#version 330 core

// Output data
out vec4 color;

// From vertex shader
in vec2 TexCoord;
in vec3 Normal;

// Texture samplers
uniform sampler2D texture1;

void main()
{
	color = texture(texture1, TexCoord);
}