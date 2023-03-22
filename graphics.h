#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"


typedef struct G_Object {
    unsigned int shader_program_id;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;

	float camera_controler[4][4];

	float vertices[8*3];
	float indices[3*2*4];

    Vec3 color;
} G_Object;

G_Object *graphics_new_object();

void graphics_free_object(G_Object *object);


#endif  // GRAPHICS_H
