#ifndef EXPERIMENT_HELPERS_H
#define EXPERIMENT_HELPERS_H


#include "../../common.h"
#include "../../graphics.h"
#include <time.h>


enum ModelType {
    ModelType_World,
    ModelType_Spoon,
    ModelType_Teacup,
    ModelType_Teapot,
    ModelType_Cube,
    ModelType_Suzanne,
    ModelType_Dragon,
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
} RandomEntity;

typedef struct GameContext {
    double current_time;
    double prev_rand_time;
    int prev_rand;
    Vec2 prev_rand_pos;
    BaseModel models[ModelType_Count];
    RandomEntity random_entities[10];
    int position_len;
    Vec3 world_center;
} GameContext;


void game_context_init(GameContext *game_ctx);
void rand_init(GameContext *game_ctx);
int get_new_random(GameContext *game_ctx);
Vec2 get_random_position(GraphicsContext *ctx, GameContext *game_ctx);
void load_assets(
    GraphicsContext *ctx, Renderer *renderer, GameContext *game_ctx, Font *font
);
void add_random_entity(GraphicsContext *ctx, GameContext *game_ctx);
void sync_entities(GameContext *game_ctx, Renderer *renderer);
void update_entities(GameContext *game_ctx);


#endif  // EXPERIMENT_HELPERS_H