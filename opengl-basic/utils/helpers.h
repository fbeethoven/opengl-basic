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
} CameraMovementParams;


void gui_quad_in_pos(
    GraphicsContext *ctx, Entity *entity, Vec2 center, float size, Vec3 color
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
    GraphicsContext *ctx, Mesh *mesh, Vec2 center, Vec3 color, float side
);
int control_is_pressed(GraphicsContext *ctx);
int shift_is_pressed(GraphicsContext *ctx);


#endif  // HELPERS_H
