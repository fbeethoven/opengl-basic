#ifndef EXPERIMENT_HELPERS_H
#define EXPERIMENT_HELPERS_H


#include "../../common.h"
#include "../../graphics.h"
#include <time.h>


enum ModelType {
    ModelType_World,
    ModelType_Cube,
    ModelType_Tea,
    ModelType_Suzanne,
    ModelType_Dragon,
    ModelType_Count
};

typedef struct GameContext {
    double current_time;
    double prev_rand_time;
    int prev_rand;
    Vec2 prev_rand_pos;
    BaseModel models[ModelType_Count];
    Vec3 positions[10];
    int position_len;
} GameContext;



void rand_init(GameContext *game_ctx);
int get_new_random(GameContext *game_ctx);
Vec2 get_random_position(GraphicsContext *ctx, GameContext *game_ctx);
void load_assets(
    GraphicsContext *ctx, Renderer *renderer, GameContext *game_ctx, Font *font
);
void add_entity(Renderer *renderer, GameContext *game_ctx, Vec3 *position);


#endif  // EXPERIMENT_HELPERS_H
