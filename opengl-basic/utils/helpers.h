#ifndef HELPERS_H
#define HELPERS_H


#include "../graphics.h"


typedef struct PlayerMovementParams {
    Entity *player;
    float rotation_factor;
    float speed;
    float d_player_move;
    double dt;
} PlayerMovementParams;


typedef struct CameraMovementParams {
    Camera *camera;
    float speed;
    float camera_speed;
    float distance_from_player;
    double dt;
    float player_rotation;
    int player_is_grounded;
} CameraMovementParams;


void quad_in_pos(
    GraphicsContext *ctx, Entity *entity, Vec3 center, Vec2 size, Vec3 color
);
void gui_quad_in_pos(
    GraphicsContext *ctx, Entity *entity, Vec2 center, Vec2 size, Vec3 color
);
void gui_quad_free(Entity *entity);
void player_movement(GraphicsContext *ctx, PlayerMovementParams *params);
void camera_movement(GraphicsContext *ctx, CameraMovementParams *params);
void camera_follow_player(
    Vec3 *pos, float rot_y, CameraMovementParams *params
);
int toggle_button_press(
    GraphicsContext *ctx, unsigned int key, int *is_press_bool
);
void free_camera_movement(GraphicsContext *ctx, CameraMovementParams *params);
void draw_quad_in_pixels(
    GraphicsContext *ctx, Mesh *mesh, Vec2 center, Vec3 color, Vec2 side
);
int control_is_pressed(GraphicsContext *ctx);
int shift_is_pressed(GraphicsContext *ctx);
void camera_reset(Camera *camera);
Vec3 mouse_to_plane(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera,
    Vec3 normal, float distance
);
Vec3 ray_to_plane_from(Vec3 origin, Vec3 toward, Vec3 normal, float distance);
Vec3 ray_to_plane(Vec3 origin, Vec3 dir, Vec3 normal, float distance);


#endif  // HELPERS_H
