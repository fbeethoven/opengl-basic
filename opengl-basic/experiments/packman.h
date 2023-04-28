#ifndef PACKMAN_H
#define PACKMAN_H


#include "../common.h"
#include "utils/experiment_helpers.h"
float speed;
int pulse_n;
int pulse_p;
int pulse_e;
float distance_from_player;
float player_is_grounded;
int show_debug_info;

int entity_index;
int entity_category_index;
char *entity_categories[3];

enum EntityCategory {
    EntityCategory_entities = 0,
    EntityCategory_gui = 1,
    EntityCategory_font = 2
};

GameContext *game_ctx;


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);


int game_run();


#endif  // PACKMAN_H
