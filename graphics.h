#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"


typedef struct GraphicsContext {
    int width;
    int height;
    GLFWwindow* window;
} GraphicsContext;


typedef struct BaseModel {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;

    int vertex_count;

    unsigned int texture_id;

} BaseModel;


typedef struct TextureModel {
    BaseModel base_model;
    unsigned int texture_id;
} TextureModel;


typedef struct Entity {
    // TODO: use a TextureModel instead
    BaseModel *model;
    Vec3 *position;
    float rotation_x;
    float rotation_y;
    float rotation_z;
    float scale;
    int active;
} Entity;


typedef struct Renderer {
    float FOV;
	float NEAR_PLANE;
	float FAR_PLANE;

	float RED;
	float GREEN;
	float BLUE;

	Mat4 projection_matrix;

	int shader;
    Entity entities[10];

    int fill;

} Renderer;


typedef struct Camera {
    float pitch;
    float yaw;
    Vec3 position;
} Camera;


void camera_move(Camera *camera, float dx, float dy, float dz);

void init_render_handler(GraphicsContext *ctx, Renderer *rh);

void increase_position(Entity *entity, float dx, float dy, float dz);

void increase_rotation(Entity *entity, float dx, float dy, float dz);

int graphics_init(GraphicsContext *ctx);

void load_texture_to_model(
    BaseModel *model, char *texture_file_path,
    float *texture_coord, int textures_size
);

void load_data_to_model(
    BaseModel *model,
    float *vertices,  unsigned int *indices,
    int vertices_size, int indices_size
);

void render(Renderer *rh, Camera *camera);


// GObject *graphics_new_object();

// void graphics_free_object(GObject *object);


#endif  // GRAPHICS_H
