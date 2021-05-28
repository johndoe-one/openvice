#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

void camera_init();
void camera_forward();
void camera_backward();
void camera_get_view(mat4 *);
void camera_get_projection(mat4 *);

#endif