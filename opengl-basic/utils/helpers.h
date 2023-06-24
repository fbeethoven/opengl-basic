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


typedef enum EditorMode {
    EditorMode_None,
    EditorMode_UI,
    EditorMode_Rotate,
    EditorMode_CreateEntity,
    EditorMode_PickEntity,
    EditorMode_EditEntity
} EditorMode;

typedef struct EditorState {
    EditorMode state;
    EditorMode prev_state;
    int ui_active;
} EditorState;

typedef struct CameraMovementParams {
    Camera *camera;
    float speed;
    float camera_speed;
    float distance_from_player;
    double dt;
    float player_rotation;
    int player_is_grounded;
    EditorState *editor_state;
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
void free_fps_camera_movement(
    GraphicsContext *ctx, CameraMovementParams *params
);
void free_rts_camera_movement(
    GraphicsContext *ctx, CameraMovementParams *params
);
void draw_quad_in_pixels(
    GraphicsContext *ctx, Mesh *mesh, Vec2 center, Vec3 color, Vec2 side
);
int control_is_pressed(GraphicsContext *ctx);
int shift_is_pressed(GraphicsContext *ctx);
void camera_reset(Camera *camera);


typedef struct RayToPlaneHit {
    int is_hit;
    Vec3 hit;
} RayToPlaneHit;

RayToPlaneHit mouse_to_plane(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera,
    Vec3 normal, float distance
);
RayToPlaneHit ray_to_plane_from(Vec3 origin, Vec3 toward, Vec3 normal, float distance);
RayToPlaneHit ray_to_plane(Vec3 origin, Vec3 dir, Vec3 normal, float distance);
int ui_button(
    GraphicsContext *ctx, Renderer *renderer, Vec2 position, char *text
);
Vec3 mouse_to_world(GraphicsContext *ctx, Renderer *renderer, Camera *camera);
int ray_to_aabb(Vec3 origin, Vec3 dir, Vec3 box_min, Vec3 box_max);
int ray_to_sphere(Vec3 origin, Vec3 dir, Vec3 center, float radius);
void ui_color_picker(GraphicsContext *ctx, Entity *entity);






// ui test
#define MeshCapacity 10

typedef struct UIWidget UIWidget;
struct UIWidget {
    // tree links
    UIWidget *first;
    UIWidget *last;
    UIWidget *next;
    UIWidget *prev;
    UIWidget *parent;

    // hash links
    UIWidget *hash_next;
    UIWidget *hash_prev;

    // key+generation info
    char key[32];
    U64 last_frame_touched_index;

    char string[32];

    Vec4 rect;
    Vec2 child_position;

    // persistent data
    float hot_t;
    float active_t;
    int hit;

    Entity *entity;
};

enum UIFlags {
    UIFlags_Visible = 1,
    UIFlags_Square = 1<<2,
    UIFlags_Clickable = 1<<3,
};

typedef struct UIManager {
    UIWidget *root_widget;
    UIWidget *current_parent_widget;
    UIWidget *current_child_widget;
    List(Entity) *gui_entities;
    UIWidget *free_widget;
    GraphicsContext *ctx;
    Renderer *renderer;
} UIManager;


typedef struct UI_InputParams {
    Renderer *renderer;
    Camera *camera;
    EditorState *state;
} UI_InputParams;



UIManager *ui_init(GraphicsContext *ctx, Renderer *renderer);
void ui_reset(UIManager *ui_manager);
void ui_test_button(UIManager *ui_manager, UI_InputParams *input);
void editor_enter_mode(EditorState *state, EditorMode new_mode);
void editor_exit_mode(EditorState *state, EditorMode exit_mode);


#endif  // HELPERS_H
