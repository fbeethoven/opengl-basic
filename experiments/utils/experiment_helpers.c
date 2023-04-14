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
    float x_pos = ctx->width * ((float)rand() / RAND_MAX);
    float y_pos = ctx->height * ((float)rand()/ RAND_MAX);
    return newVec2(x_pos, y_pos);
}

void load_assets(
    GraphicsContext *ctx, Renderer *renderer, GameContext *game_ctx, Font *font
) {
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    int mesh_size = 64*64;

    mesh->vertices = (Vec3 *)malloc(mesh_size * sizeof(Vec3));
    mesh->uvs = (Vec2 *)malloc(mesh_size * sizeof(Vec2));
    mesh->normal = (Vec3 *)malloc(mesh_size * sizeof(Vec3));
    mesh->indices = (unsigned int*)malloc(mesh_size * sizeof(unsigned int));
    mesh_init(mesh);

    BaseModel *world_model = &game_ctx->models[ModelType_World];
    load_data_to_model(
        world_model, (float *) mesh->vertices, mesh->indices,
        3*sizeof(float)*mesh->vertices_len,
        sizeof(unsigned int)*mesh->indices_len
    );
    load_texture_to_model(
        world_model,
        // "assets/textures/marble-floor.jpg",
        "assets/textures/wall.jpg",
        // "assets/textures/wood-floor.jpg",
        (float *)mesh->uvs, 
        2 * sizeof(float) * mesh->uvs_len
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(world_model->vao);
    store_float_in_attributes(
        &world_model->normal,
        2,
        3,
        3 * mesh->vertices_len * sizeof(float),
        (float *) mesh->normal
    );
    log_if_err("Issue after loading normals\n");

    world_model->vertex_count = mesh->indices_len;

    free(mesh->vertices);
    free(mesh->uvs);
    free(mesh->normal);
    free(mesh->indices);
    free(mesh);


    log_if_err("Issue before Font initiation\n");
    float aspect_ratio = (float)ctx->width / (float)ctx->height;
    font_init(font, "assets/fonts/VictorMono-Regular.ttf", aspect_ratio);
    renderer->font = font;
    log_if_err("Issue with Font initiation\n");


    Entity *entity = &renderer->font_entities[0];
    strcpy(entity->debug_name, "Default Font");
    entity->model = (BaseModel *) &font->vao;
    Vec3 *world_center = &game_ctx->positions[game_ctx->position_len++];
    *world_center = newVec3(0.0, 0.0, 0.0);
    entity->position = world_center;
    entity->active = 1;
    entity->scale = 1;

    entity = &renderer->entities[1];
    strcpy(entity->debug_name, "World Floor");
    entity->model = world_model;
    entity->position = world_center;
    entity->active = 1;
    entity->scale = 1.0;


    BaseModel *cube_model = &game_ctx->models[ModelType_Cube];
    IntermediateModel cube_data = {0};
    intermediate_model_init(&cube_data);
    parse_obj_file_simple("assets/models/cube.obj", &cube_data);
    load_data_to_model(
        cube_model, cube_data.vertices, cube_data.indices,
        cube_data.vertices_count * sizeof(float),
        cube_data.indices_count * sizeof(unsigned int)
    );
    cube_model->vertex_count = cube_data.indices_count;
    intermediate_model_free(&cube_data);
    float text_coord1[] = {
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f,

        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f,

        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f,

        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f,

        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f,

        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f,

    };
    load_texture_to_model(
        cube_model, "assets/textures/wood-floor.jpg", text_coord1, 
        sizeof(text_coord1)
    );

    BaseModel *tea_model = &game_ctx->models[ModelType_Tea];
    IntermediateModel tmp = {0};
    intermediate_model_init(&tmp);
    parse_obj_file_simple("assets/models/utah_teapot.obj", &tmp);
    load_data_to_model(
        tea_model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    tea_model->vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);


    BaseModel *suzanne = &game_ctx->models[ModelType_Suzanne];
    IntermediateModel suzanne_data = {0};
    parse_obj_file("assets/models/suzanne.obj", &suzanne_data);
    load_data_to_model(
        suzanne, suzanne_data.vertices, suzanne_data.indices,
        suzanne_data.vertices_count* sizeof(float),
        suzanne_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        suzanne, "assets/textures/wood-floor.jpg", suzanne_data.uvs, 
        suzanne_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(suzanne->vao);
    store_float_in_attributes(
        &suzanne->normal,
        2,
        3,
        suzanne_data.normals_count * sizeof(float),
        suzanne_data.normals
    );
    log_if_err("Issue after loading normals\n");
    suzanne->vertex_count = suzanne_data.indices_count;
    intermediate_model_free(&suzanne_data);

    BaseModel *dragon = &game_ctx->models[ModelType_Dragon];
    IntermediateModel dragon_data = {0};
    parse_obj_file("assets/models/dragon.obj", &dragon_data);
    load_data_to_model(
        dragon, dragon_data.vertices, dragon_data.indices,
        dragon_data.vertices_count* sizeof(float),
        dragon_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        dragon, "assets/textures/wood-floor.jpg", dragon_data.uvs, 
        dragon_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(dragon->vao);
    store_float_in_attributes(
        &dragon->normal,
        2,
        3,
        dragon_data.normals_count * sizeof(float),
        dragon_data.normals
    );
    log_if_err("Issue after loading normals\n");
    dragon->vertex_count = dragon_data.indices_count;
    intermediate_model_free(&dragon_data);
}


void add_entity(Renderer *renderer, GameContext *game_ctx, Vec3 *position) {
    Entity *entity;
    int found = 0;
    for (int i=0; i<10; i++) {
        entity = &renderer->entities[i];
        if (entity->active == 0) {
            strcpy(entity->debug_name, "First Entity");
            entity->model = &game_ctx->models[ModelType_Suzanne];

            Vec3 *pos = &game_ctx->positions[i];
            *pos = newVec3(0.0, 5.0, 0.0);
            entity->position = pos;

            entity->active = 1;
            entity->scale = 1.0;
            found = 1;
            return;
        }
    }
    if (!found) {
        int ind = rand() % 10;
        entity = &renderer->entities[ind];
        entity->active = 0;
    }
}


void rand_init(GameContext *game_ctx) {
    double time_v = glfwGetTime();
    game_ctx->current_time = time_v;
    game_ctx->prev_rand_time = 0.0;
    game_ctx->prev_rand = 0;
    srand(time(0));

}
