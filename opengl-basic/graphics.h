#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"
#include "mesh.h"
#include "font.h"
#include "shader.h"
#include "config.h"


typedef struct GraphicsContext {
    int width;
    int height;
    int resolution_has_changed;
    double current_time;
    double dtime;
    double game_time;
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
    Vec3 position;
    Vec3 color;
    Vec3 rotation;
    Vec3 scale;
    int active;
    int fill;
    char debug_name[32];
} Entity;


LIST_ADD(BaseModel);
LIST_ADD(Entity);


typedef struct Renderer {
    float FOV;
	float NEAR_PLANE;
	float FAR_PLANE;

	float RED;
	float GREEN;
	float BLUE;

	Mat4 projection_matrix;

	int shader;
    int anim_shader;
	int circle_shader;
	int gui_shader;
	int sky_shader;

#if 0
    Entity entities[20];
    Entity debug_entities[100];
    Entity gui_entities[10];
    Entity font_entities[10];
#endif

    List(Entity) *entities;
    List(Entity) *debug_entities;

    List(Entity) *gui_entities;
    List(Entity) *font_entities;

    Font *font;
    Light *light;

    BaseModel skybox;

} Renderer;


typedef struct Camera {
    float pitch;
    float yaw;
    Vec3 position;
    Vec3 centre;
} Camera;


void camera_move(Camera *camera, float dx, float dy, float dz);

void init_render_handler(GraphicsContext *ctx, Renderer *rh);

int graphics_init(GraphicsContext *ctx);

void load_texture_to_model(
    BaseModel *model, char *texture_file_path,
    float *texture_coord, int textures_size
);

void load_empty_texture_to_model(
    BaseModel *model, float *texture_coord, int textures_size
);


void load_data_to_model(
    BaseModel *model, float *vertices,  unsigned int *indices,
    int vertices_size, int indices_size
);

void render(Renderer *rh, Camera *camera);

void store_float_in_attributes(
    unsigned int *buffer_id, int attribute_index, int coordinate_size,
    int buffer_size, float *data
);

void store_int_in_attributes(
    unsigned int *buffer_id, int attribute_index, int coordinate_size,
    int buffer_size, int *data
);

void bind_indices_buffer(
    unsigned int *buffer_id, int buffer_size, unsigned int *data
);
void reload_projection_matrix(GraphicsContext *ctx, Renderer *rh);

void init_floor_model(BaseModel *world_model);
void init_font(GraphicsContext *ctx, Renderer *renderer, Font *font);
void load_model_from_obj(BaseModel *model, char *obj_file, char *texture_file);
void load_model_from_gltf(
    BaseModel *model, char *gltf_file, char *bin_file, char *texture_file
);


#endif  // GRAPHICS_H
