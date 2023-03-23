#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"


typedef struct GraphicsContext {
    GLFWwindow* window;
} GraphicsContext;


typedef struct G_Object {
    unsigned int shader_program_id;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;

	unsigned int texture_id;

	float camera_controler[4][4];

    Vec3 color;
} G_Object;


G_Object *graphics_new_object();

void graphics_free_object(G_Object *object);

int graphics_init(GraphicsContext *ctx);

G_Object *graphics_new_rect(
    GraphicsContext *ctx,
    Vec3 *topright, Vec3 *topleft, Vec3 *botleft, Vec3 *botright
);

void graphics_render_rect(GraphicsContext *ctx, G_Object *object);


#endif  // GRAPHICS_H
