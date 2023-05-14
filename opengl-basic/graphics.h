#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"
#include "mesh.h"
#include "font.h"
#include "shader.h"


typedef struct GraphicsContext {
    int width;
    int height;
    double previous_time;
    double mouse_position[2];
    double dmouse[2];
    GLFWwindow* window;
} GraphicsContext;


typedef struct BaseModel {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
    unsigned int uv;
    unsigned int color;
    unsigned int normal;

    int vertex_count;

    unsigned int texture_id;

} BaseModel;


typedef struct Entity {
    BaseModel *model;
    Vec3 *position;
    Vec3 color;
    float rotation_x;
    float rotation_y;
    float rotation_z;
    Vec3 scale;
    int active;
    int fill;
    char debug_name[50];
} Entity;

typedef struct Transform {
    Vec3 translation;
    Vec3 rotation;
    Vec3 scale;
} Transform;

typedef struct Joint Joint;

typedef struct Joint {
    Transform local_transform;
    Joint *children;
    Joint *next;

    Entity *entity;
} Joint;


typedef struct Renderer {
    float FOV;
	float NEAR_PLANE;
	float FAR_PLANE;

	float RED;
	float GREEN;
	float BLUE;

	Mat4 projection_matrix;

	int shader;
    Entity entities[20];
    Entity debug_entities[100];

	int circle_shader;
	int gui_shader;
    Entity gui_entities[10];
    Entity font_entities[10];

    Font *font;
    Light *light;

    Joint *root;

} Renderer;


typedef struct Camera {
    float pitch;
    float yaw;
    Vec3 position;
    Vec3 centre;
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

void load_empty_texture_to_model(
    BaseModel *model, float *texture_coord, int textures_size
);


void load_data_to_model(
    BaseModel *model,
    float *vertices,  unsigned int *indices,
    int vertices_size, int indices_size
);

void render(Renderer *rh, Camera *camera);

void store_float_in_attributes(
    unsigned int *buffer_id,
    int attribute_index,
    int coordinate_size,
    int buffer_size,
    float *data
);

void bind_indices_buffer(
    unsigned int *buffer_id,
    int buffer_size,
    unsigned int *data
);
void reload_projection_matrix(GraphicsContext *ctx, Renderer *rh);

Joint *new_joint(Entity *entity);
// void joint_update(Joint *parent, Joint *joint);
void joint_update_all(Joint *root);
// void joint_update_children(Joint *root);
Joint *joint_push(Joint *joint, Entity *entity);

#endif  // GRAPHICS_H
