#ifndef EXPERIMENT_HELPERS_H
#define EXPERIMENT_HELPERS_H


#include "../../common.h"
#include "../../graphics.h"
#include "../../utils/helpers.h"
#include <time.h>


#define ResevedModels 2

enum ModelType {
    ModelType_World,
    ModelType_Spoon,
    ModelType_Teacup,
    ModelType_Teapot,
    ModelType_Cube,
    ModelType_Suzanne,
    ModelType_Dragon,
    ModelType_Blendercube,
    ModelType_sphere,
    ModelType_Trooper,
    ModelType_Count
};

typedef struct RandomEntity {
    Entity *entity;
    int *active;
    Vec3 position;
    Vec3 start;
    Vec3 dest;
    double end_time;
    double start_time;
    int is_grounded;
} RandomEntity;

typedef struct GameContext {
    double current_time;
    double prev_rand_time;
    double start_time;
    int prev_rand;
    Vec2 prev_rand_pos;
    BaseModel models[ModelType_Count];
    RandomEntity random_entities[20];
    int position_len;
    Vec3 world_center;
    int game_over;
    int points;
    int max_points;
    char msg[100];
} GameContext;


void game_context_init(GameContext *game_ctx);
void rand_init(GameContext *game_ctx);
int get_new_random(GameContext *game_ctx);
Vec2 get_random_position(GraphicsContext *ctx, GameContext *game_ctx);
void load_assets(
    GraphicsContext *ctx, Renderer *renderer, GameContext *game_ctx, Font *font,
    Camera *camera

);
RandomEntity* add_random_entity(
    GraphicsContext *ctx, GameContext *game_ctx, Camera *camera
);
void sync_entities(GameContext *game_ctx, Renderer *renderer);
void update_entities(GameContext *game_ctx, Camera *camera);
void new_circle_entity(GameContext *game_ctx);
Entity* get_new_debug_entity(
    GameContext *game_ctx, Renderer *renderer, Vec3 pos
);


#endif  // EXPERIMENT_HELPERS_H
