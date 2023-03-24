#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"


typedef struct GraphicsContext {
    int width;
    int height;
    GLFWwindow* window;
} GraphicsContext;


typedef struct Object {
    unsigned int shader_program_id;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;

	unsigned int texture_id;

	float camera_controler[4][4];

    Vec3 color;
} GObject;

typedef RenderHandler {
    float FOV;
	float NEAR_PLANE;
	float FAR_PLANE;

	float RED;
	float GREEN;
	float BLUE;

	Mat4 projectionMatrix;

	int shader;
	Renderer renderer;

} RenderHandler;

typedef Camera {
    float pitch;
    float yaw;
    Vec3 position;
} Camera;

void init_render_handler();

GObject *graphics_new_object();

void graphics_free_object(GObject *object);

int graphics_init(GraphicsContext *ctx);

G_Object *graphics_new_rect(
    GraphicsContext *ctx,
    Vec3 *topright, Vec3 *topleft, Vec3 *botleft, Vec3 *botright
);

void graphics_render_rect(GraphicsContext *ctx, GObject *object);


#endif  // GRAPHICS_H
