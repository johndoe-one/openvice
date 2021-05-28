#include "camera.h"

static vec3 eye = {0.0, 0.0, -5.0};
static vec3 center = {0.0, 0.0, 0.0};

void camera_init()
{
}

void camera_forward()
{
	vec3 forward = {0.0, 0.0, 1.0};

	vec3 new_pos;
	glm_vec3_add(eye, forward, eye);
}

void camera_backward()
{
	vec3 backward = { 0.0, 0.0, -1.0 };

	vec3 new_pos;
	glm_vec3_add(eye, backward, eye);
}

void camera_get_view(mat4 *v)
{
	vec3 up = { 0.0, 1.0, 0.0 };

	glm_lookat(eye, center, up, v);
}

void camera_get_projection(mat4* proj)
{
	glm_perspective(90.0, 1024.0 / 768.0, 0.1, 1000.0, proj);
}