#include "experiment_helpers.h"


int get_new_random(GameContext *game_ctx) {
    double time_v = glfwGetTime();
    if (time_v - game_ctx->prev_rand_time > 1.0) {
        game_ctx->prev_rand_time = time_v;
        game_ctx->prev_rand = rand();
        return 1;
    }
    game_ctx->current_time = time_v;
    return 0;
}

void load_model_cube(GameContext *game_ctx) {
}

Vec2 get_random_position(GraphicsContext *ctx, GameContext *game_ctx) {
    float x_pos = 200.0*((float)rand() / RAND_MAX) - 100.0;
    float y_pos = 200.0*((float)rand() / RAND_MAX) - 100.0;
    return newVec2(x_pos, y_pos);
}

void load_assets(
    GraphicsContext *ctx, Renderer *renderer, GameContext *game_ctx,
    Font *font, Camera *camera
) {

    BaseModel *model = &game_ctx->models[ModelType_World];
    init_floor_model(model);
    init_font(ctx, renderer, font);

    model = &game_ctx->models[ModelType_Spoon];
    load_model_from_obj(
        model, "assets/models/spoon.obj", "assets/textures/wood-floor.jpg"
    );

    model = &game_ctx->models[ModelType_Teacup];
    load_model_from_obj(
        model, "assets/models/teacup.obj", "assets/textures/wood-floor.jpg"
    );


    model = &game_ctx->models[ModelType_Teapot];
    load_model_from_obj(
        model, "assets/models/teapot.obj", "assets/textures/wood-floor.jpg"
    );


    model = &game_ctx->models[ModelType_Suzanne];
    load_model_from_obj(
        model, "assets/models/suzanne.obj", "assets/textures/wood-floor.jpg"
    );

    model = &game_ctx->models[ModelType_Dragon];
    load_model_from_obj(
        model, "assets/models/dragon.obj",
        "assets/textures/Stormtroopermat_baseColor.png"
    );


    model = &game_ctx->models[ModelType_Blendercube];
    load_model_from_obj(
        model, "assets/models/BlenderCube.obj",
        "assets/textures/wood-floor.jpg"
    );

    model = &game_ctx->models[ModelType_sphere];
    load_model_from_obj(
        model, "assets/models/sphere.obj", "assets/textures/wood-floor.jpg"
    );

    model = &game_ctx->models[ModelType_Cube];
    load_model_from_obj(
        model, "assets/models/cube.obj", "assets/textures/wood-floor.jpg"
    );

    model = &game_ctx->models[ModelType_Trooper];
    load_model_from_gltf(
        model, "assets/models/scene.gltf", "assets/models/scene.bin",
        "assets/textures/Stormtroopermat_baseColor.png"
    );


    Entity *entity;
    entity = &renderer->font_entities[0];
    strcpy(entity->debug_name, "Default Font");
    entity->position = newVec3(0.0, 0.0, 0.0);
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);
    entity->model = (BaseModel *) &font->vao;

    entity = &renderer->entities[0];
    strcpy(entity->debug_name, "World Floor");
    entity->model = &game_ctx->models[ModelType_World];
    entity->position = newVec3(0.0, 0.0, 0.0);
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);

    entity = &renderer->entities[1];
    strcpy(entity->debug_name, "Storm Trooper");
    entity->model = &game_ctx->models[ModelType_Trooper];
    entity->position = newVec3(0.0, 0.0, 0.0);
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);

    entity = &renderer->entities[2];
    entity->model = &game_ctx->models[ModelType_Cube];
    entity->position = newVec3(1.0, 10.0, 1.0);
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);

}


Entity* get_new_debug_entity(
    GameContext *game_ctx, Renderer *renderer, Vec3 pos
) {
    Entity *entity;
    for (int i=0; i<100; i++) {
        entity = &renderer->debug_entities[i];
        if (entity->active == 0) {
            sprintf(entity->debug_name, "Debug Entity %d", i); 

            entity->model = &game_ctx->models[ModelType_sphere];

            entity->position = pos;

            entity->active = 1;
            entity->color = newVec3(1.0, 1.0, 0.0);
            entity->scale = newVec3(0.05, 0.05, 0.05);
            return entity;
        }
    }
    return 0;
}


RandomEntity* get_random_entity_slot(
    GraphicsContext *ctx, GameContext *game_ctx, Camera *camera
) {
    RandomEntity *r_entity;
    for (int i=0; i<10; i++) {
        r_entity = &game_ctx->random_entities[i];
        if (*r_entity->active == 0) {
            char msg[50];
            sprintf(msg, "Entity %0.3f", game_ctx->current_time);
            strcpy(r_entity->entity->debug_name, msg);
            return r_entity;
        }
    }
    return 0;
}

RandomEntity* add_random_entity(
    GraphicsContext *ctx, GameContext *game_ctx, Camera *camera
) {
    RandomEntity *r_entity;
    for (int i=0; i<10; i++) {
        r_entity = &game_ctx->random_entities[i];
        if (*r_entity->active == 0) {
            sprintf(
                r_entity->entity->debug_name,
                "RANDOM %0.3f", game_ctx->current_time
            );
            int ran_model = (
                (rand() % (ModelType_Count - ResevedModels)) + ResevedModels
            );
            r_entity->entity->model = &game_ctx->models[ran_model];

            float scalar;
            if (ran_model == ModelType_Dragon) { scalar = 1.2; } 
            else if (ran_model == ModelType_Spoon){ scalar = 10.0;}
            else if (ran_model == ModelType_Teapot){ scalar = 1.5;}
            else { scalar = 3.0;}
            r_entity->entity->scale = newVec3(scalar, scalar, scalar);

            float rot = (float)(rand() % 3000000)/3000000;
            r_entity->entity->rotation.y = rot * 3.1415;

            double t = game_ctx->current_time;
            r_entity->start_time = t;
            r_entity->end_time = t + (double)(rand() % 9) + 5.0;
            Vec2 ran_pos = get_random_position(ctx, game_ctx);
            
            r_entity->position.x = ran_pos.x;
            r_entity->position.y = scalar;
            r_entity->position.z = ran_pos.y;

            r_entity->start.x = ran_pos.x;
            r_entity->start.y = scalar;
            r_entity->start.z = ran_pos.y;

            r_entity->dest.x = camera->position.x;
            r_entity->dest.y = scalar;
            r_entity->dest.z = camera->position.z;
            *r_entity->active = 1;
            return r_entity;
        }
    }
    return 0;
}


void rand_init(GameContext *game_ctx) {
    double time_v = glfwGetTime();
    game_ctx->current_time = time_v;
    game_ctx->prev_rand_time = 0.0;
    game_ctx->prev_rand = 0;
    srand(time(0));

}


void sync_entities(GameContext *game_ctx, Renderer *renderer) {
    Entity *entity;
    RandomEntity *rand_entity;
    for (int i=0; i<20; i++) {
        entity = &renderer->entities[i];
        rand_entity = &game_ctx->random_entities[i];
        rand_entity->entity = entity;
        rand_entity->active = &entity->active;
        entity->position = rand_entity->position;
        entity->scale = newVec3(1.0, 1.0, 1.0);
        entity->active = 0;
    }
    rand_init(game_ctx);
}

void update_entities(GameContext *game_ctx, Camera *camera) {
    RandomEntity *r_entity;
    for (int i=1; i<10; i++) {
        r_entity = &game_ctx->random_entities[i];
        if (*r_entity->active) {
            if (game_ctx->current_time >= r_entity->end_time) {
                *r_entity->active = 0;
                game_ctx->points++;
                continue;
            }
            Vec3 dir = newVec3(
                camera->position.x - r_entity->position.x,
                0.0,
                camera->position.z - r_entity->position.z
            );
            vec3_normalize(&dir);
            r_entity->position.x += dir.x;
            r_entity->position.z += dir.z;

            float dis = vec3_distance(&r_entity->position, &camera->position);
            if (dis < 2.0) {
                game_ctx->game_over = 1;
            }
        }
    }
}

void new_circle_entity(GameContext *game_ctx) {
    RandomEntity *entity;
    for (int i=10; i<20; i++) {
        entity = &game_ctx->random_entities[i];
        if (*entity->active == 0) {
            entity->entity->model = &game_ctx->models[ModelType_sphere];
            entity->position = newVec3(0.0, 0.0, 0.0);
            entity->entity->scale = newVec3(1.0, 1.0, 1.0);
            entity->entity->color = newVec3(1.0, 1.0, 0.0);
            *entity->active = 1;
            return;
        }
    }
}

