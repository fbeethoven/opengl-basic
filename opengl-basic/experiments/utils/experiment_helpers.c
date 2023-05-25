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
    GraphicsContext *ctx, Renderer *renderer, GameContext *game_ctx, Font *font, Camera *camera
) {
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    int mesh_size = 128 * 128;

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
    font_init(
        font, "assets/fonts/VictorMono-Regular.ttf",
        (float)ctx->width, (float)ctx->height
    );
    renderer->font = font;
    log_if_err("Issue with Font initiation\n");


    Entity *entity = &renderer->font_entities[0];
    strcpy(entity->debug_name, "Default Font");
    entity->model = (BaseModel *) &font->vao;
    Vec3 *font_position = malloc(sizeof(Vec3));
    entity->position = font_position;
    *entity->position = newVec3(0.0, 0.0, 0.0);
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);

    entity = &renderer->entities[0];
    strcpy(entity->debug_name, "World Floor");
    entity->model = world_model;
    entity->position = &game_ctx->world_center;
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);


    BaseModel *cube_model = &game_ctx->models[ModelType_Cube];
    IntermediateModel cube_data = {0};
    parse_obj_file_simple("assets/models/cube.obj", &cube_data);
    load_data_to_model(
        cube_model, cube_data.vertices, cube_data.indices,
        cube_data.vertices_count * sizeof(float),
        cube_data.indices_count * sizeof(unsigned int)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(cube_model->vao);
    store_float_in_attributes(
        &cube_model->normal,
        2,
        3,
        cube_data.normals_count * sizeof(float),
        cube_data.normals
    );
    log_if_err("Issue after loading normals\n");
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


    BaseModel *model = &game_ctx->models[ModelType_Spoon];
    IntermediateModel tmp = {0};
    parse_obj_file("assets/models/spoon.obj", &tmp);
    load_data_to_model(
        model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        model, "assets/textures/wood-floor.jpg", tmp.uvs, 
        tmp.uvs_count * sizeof(float)
    );
    glBindVertexArray(model->vao);
    store_float_in_attributes(
        &model->normal,
        2,
        3,
        tmp.normals_count * sizeof(float),
        tmp.normals
    );
    log_if_err("Issue after loading normals\n");
    model->vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);


    model = &game_ctx->models[ModelType_Teacup];
    parse_obj_file("assets/models/teacup.obj", &tmp);
    load_data_to_model(
        model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        model, "assets/textures/wood-floor.jpg", tmp.uvs, 
        tmp.uvs_count * sizeof(float)
    );
    glBindVertexArray(model->vao);
    store_float_in_attributes(
        &model->normal,
        2,
        3,
        tmp.normals_count * sizeof(float),
        tmp.normals
    );
    log_if_err("Issue after loading normals\n");
    model->vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);


    model = &game_ctx->models[ModelType_Teapot];
    parse_obj_file("assets/models/teapot.obj", &tmp);
    load_data_to_model(
        model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        model, "assets/textures/wood-floor.jpg", tmp.uvs, 
        tmp.uvs_count * sizeof(float)
    );
    glBindVertexArray(model->vao);
    store_float_in_attributes(
        &model->normal,
        2,
        3,
        tmp.normals_count * sizeof(float),
        tmp.normals
    );
    log_if_err("Issue after loading normals\n");
    model->vertex_count = tmp.indices_count;
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
        // dragon, "assets/textures/wood-floor.jpg", dragon_data.uvs, 
        dragon, "assets/textures/Stormtroopermat_baseColor.png",
        dragon_data.uvs,
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


    float a = 0.0;
    float b = 1.0;

    float vertices[] = {
        -b, b, a,
        -b, -b, a,
        b, -b, a,
        b, b, a,

        a, b, -b,
        a, -b, -b,
        a, -b, b,
        a, b, b,

        b, a, -b,
        -b, a, -b,
        -b, a, b,
        b, a, b
    };

    float uvs[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,

        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,

        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0

    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,

        8, 9, 10,
        10, 11, 8
    };

    BaseModel *sphere = &game_ctx->models[ModelType_SimpleSphere];
    load_data_to_model(
        sphere, vertices, indices, sizeof(vertices), sizeof(indices)
    );
    load_empty_texture_to_model(sphere, uvs, sizeof(uvs));
    sphere->vertex_count = sizeof(indices)/sizeof(unsigned int);


    BaseModel *blendercube = &game_ctx->models[ModelType_Blendercube];
    IntermediateModel blendercube_data = { 0 };
    parse_obj_file("assets/models/BlenderCube.obj", &blendercube_data);
    load_data_to_model(
        blendercube, blendercube_data.vertices, blendercube_data.indices,
        blendercube_data.vertices_count * sizeof(float),
        blendercube_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        blendercube, "assets/textures/wood-floor.jpg", blendercube_data.uvs,
        blendercube_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(blendercube->vao);
    store_float_in_attributes(
        &blendercube->normal,
        2,
        3,
        blendercube_data.normals_count * sizeof(float),
        blendercube_data.normals
    );
    log_if_err("Issue after loading normals\n");
    blendercube->vertex_count = blendercube_data.indices_count;
    intermediate_model_free(&blendercube_data);


    sphere = &game_ctx->models[ModelType_sphere];
    IntermediateModel sphere_data = { 0 };
    parse_obj_file("assets/models/sphere.obj", &sphere_data);
    load_data_to_model(
        sphere, sphere_data.vertices, sphere_data.indices,
        sphere_data.vertices_count * sizeof(float),
        sphere_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        sphere, "assets/textures/wood-floor.jpg", sphere_data.uvs,
        sphere_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(sphere->vao);
    store_float_in_attributes(
        &sphere->normal,
        2,
        3,
        sphere_data.normals_count * sizeof(float),
        sphere_data.normals
    );
    log_if_err("Issue after loading normals\n");
    sphere->vertex_count = sphere_data.indices_count;
    intermediate_model_free(&sphere_data);





#if 1
    AnimatedModel *anim_trooper = get_animated_model();
    BaseModel *trooper = (BaseModel *) anim_trooper;

    char *data = read_file("assets/models/scene.gltf");
    GltfData gltf = parse_gltf_data(data);
    char *binary = read_file("assets/models/scene.bin");

    IntermediateModel trooper_data = load_data_from_gltf(&gltf, binary);
    free(data);
    free(binary);
    

    load_data_to_model(
        trooper, trooper_data.vertices, trooper_data.indices,
        trooper_data.vertices_count * sizeof(float),
        trooper_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        trooper, "assets/textures/Stormtroopermat_baseColor.png",
        trooper_data.uvs,
        trooper_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(trooper->vao);
    store_float_in_attributes(
        &trooper->normal,
        2,
        3,
        trooper_data.normals_count * sizeof(float),
        trooper_data.normals
    );
    log_if_err("Issue after loading normals\n");
    trooper->vertex_count = trooper_data.indices_count;
    intermediate_model_free(&trooper_data);


    glBindVertexArray(trooper->vao);
    store_int_in_attributes(&anim_trooper->jbo, 3, 4,
        anim_trooper->joints->counter * sizeof(int),
        (int *)anim_trooper->joints->data
    );
    log_if_err("Issue loading joints\n");
    glBindVertexArray(trooper->vao);
    store_float_in_attributes(&anim_trooper->wbo, 4, 4,
        anim_trooper->weights->counter * sizeof(float),
        (float *)anim_trooper->weights->data
    );
    log_if_err("Issue loading weights\n");


    entity = &renderer->debug_entities[99];
    strcpy(entity->debug_name, "Storm Trooper");
    entity->model = trooper;

    Vec3 *pos = malloc(sizeof(Vec3));
    *pos = newVec3(0.0, 0.0, 0.0);
    entity->position = pos;

    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);
#endif



#if 0

    Vec3 pos_a = newVec3(0.0, 1.0, 0.0);
    Entity *e_root = get_new_debug_entity(game_ctx, renderer, pos_a);
    Joint *root = new_joint(e_root);
    root->local_transform.rotation = newVec3(0.0, 0.0, 1.5707);

    renderer->root = root;


    Joint *j_child = joint_push(
        root, get_new_debug_entity(game_ctx, renderer, newVec3(0.0, 0.0, 0.0))
    );


    j_child->local_transform.translation = newVec3(1.0, 0.0, 0.0);

#endif


    renderer->animation_controller = calloc(1, sizeof(AnimationController));
    renderer->animation_controller->joints = get_anim_bones();

    animation_update(renderer->animation_controller, 0.0);
    
#if 1
    ArrayList *joints = renderer->animation_controller->joints;
    for(int i=0; i<joints->counter; i++) {

        Vec4 origin = newVec4(0.0, 0.0, 0.0, 1.0);
        Joint joint = arr_get(joints, Joint, i);
        Mat4 C = joint.inverse_bind_matrix;
        C = mat4_inverse(&C);
        C = mat4_transpose(&C);

        Vec4 result = vec4_multiply(&C, &origin);
        Vec3 position = newVec3(result.x, result.y, result.z);
        get_new_debug_entity(game_ctx, renderer, position);
    }
#endif


    (&renderer->entities[0])->active = 0;


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

            Vec3 *new_pos = malloc(sizeof(Vec3));
            *new_pos = pos;
            entity->position = new_pos;

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
            r_entity->entity->rotation_y = rot * 3.1415;

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
        entity->position = &rand_entity->position;
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
            entity->entity->model = &game_ctx->models[ModelType_SimpleSphere];
            entity->position = newVec3(0.0, 0.0, 0.0);
            entity->entity->scale = newVec3(1.0, 1.0, 1.0);
            entity->entity->color = newVec3(1.0, 1.0, 0.0);
            *entity->active = 1;
            return;
        }
    }
}

