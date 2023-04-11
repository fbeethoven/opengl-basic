#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "common.h"
#include "graphics.h"
#include "mesh.h"
#include "utils/file_handler.h"
#include "font.h"


float speed;
int entity_index;
int pulse_n;

float distance_from_player;


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);


int main() {
    Font font = {0};

    speed = 0.5;
    entity_index = 0;
    pulse_n = 0;

    distance_from_player = 5.0;

    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(5.0, 0.0, 0.0);
    camera.centre = newVec3(0.0, 0.0, 0.0);
    camera.pitch = 0.1745;
    camera.yaw = 0.0;

    Light light = {0};
    light.position = newVec3(0.0, -1.0, 0.0);
    light.color = newVec3(0.8, 0.8, 1.0);

    renderer.light = &light;



    // mesh usage example
    Mesh mesh = {0};

    Vec3 mesh_vertices[64*64];
    Vec2 uvs[64*64];
    Vec3 mesh_normal[64*64];
    unsigned int mesh_indices[3*64*64];

    mesh.vertices = mesh_vertices;
    mesh.uvs = uvs;
    mesh.indices = mesh_indices;
    mesh.normal = mesh_normal;
    mesh_init(&mesh);


    BaseModel world_model = {0};
    load_data_to_model(
        &world_model, (float *) mesh.vertices, mesh.indices,
        3*sizeof(float)*mesh.vertices_len,
        sizeof(unsigned int)*mesh.indices_len
    );
    load_texture_to_model(
        &world_model,
        "assets/textures/marble-floor.jpg",
        // "assets/textures/wall.jpg",
        // "assets/textures/wood-floor.jpg",
        (float *)mesh.uvs, 
        2 * sizeof(float) * mesh.uvs_len
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(world_model.vao);
    store_float_in_attributes(
        &world_model.normal,
        2,
        3,
        3 * mesh.vertices_len * sizeof(float),
        (float *) mesh.normal
    );
    log_if_err("Issue after loading normals\n");

    world_model.vertex_count = mesh.indices_len;

    
    // float vertices1[] = {
    //     -0.5f,-0.5f, 0.0f,
    //     -0.5f,0.5f, 0.0f,
    //     0.5f,0.5f, 0.0f,
    //     0.5f,-0.5f, 0.0f
    // };
    
    float text_coord1[] = {
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f
	};

    // unsigned int indices1[] = {
    //     0, 1, 2,
    //     0, 2, 3
    // };

    // BaseModel rect = {0};
    // load_data_to_model(
    //     &rect, (float *)vertices1, indices1,
    //     3 * sizeof(float) * 4, sizeof(indices1)
    // );
    // rect.vertex_count = sizeof(indices1)/sizeof(indices1[0]);
    // load_texture_to_model(
    //     // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
    //     &rect, "assets/textures/marble-floor.jpg", text_coord1, 
    //     sizeof(text_coord1)
    // );



    log_if_err("Issue before Font initiation\n");
    float aspect_ratio = (float)ctx.width / (float)ctx.height;
    font_init(&font, "assets/fonts/VictorMono-Regular.ttf", aspect_ratio);
    renderer.font = &font;
    log_if_err("Issue with Font initiation\n");


    BaseModel cube_model = {0};
    IntermediateModel cube_data = {0};
    intermediate_model_init(&cube_data);
    parse_obj_file_simple("assets/models/cube.obj", &cube_data);
    load_data_to_model(
        &cube_model, cube_data.vertices, cube_data.indices,
        cube_data.vertices_count * sizeof(float),
        cube_data.indices_count * sizeof(unsigned int)
    );
    cube_model.vertex_count = cube_data.indices_count;
    intermediate_model_free(&cube_data);
    load_texture_to_model(
        // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
        &cube_model, "assets/textures/wall.jpg", text_coord1, 
        sizeof(text_coord1)
    );

    BaseModel tea_model = {0};
    IntermediateModel tmp = {0};
    intermediate_model_init(&tmp);
    parse_obj_file_simple("assets/models/utah_teapot.obj", &tmp);
    load_data_to_model(
        &tea_model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    tea_model.vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);


    BaseModel suzanne = {0};
    IntermediateModel suzanne_data = {0};
    parse_obj_file("assets/models/suzanne.obj", &suzanne_data);
    //parse_obj_file("assets/models/dragon.obj", &suzanne_data);
    load_data_to_model(
        &suzanne, suzanne_data.vertices, suzanne_data.indices,
        suzanne_data.vertices_count* sizeof(float),
        suzanne_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
        &suzanne, "assets/textures/wall.jpg", suzanne_data.uvs, 
        suzanne_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(suzanne.vao);
    store_float_in_attributes(
        &suzanne.normal,
        2,
        3,
        suzanne_data.normals_count * sizeof(float),
        suzanne_data.normals
    );
    log_if_err("Issue after loading normals\n");


    suzanne.vertex_count = suzanne_data.indices_count;
    intermediate_model_free(&suzanne_data);


    Entity *entity = &renderer.gui_entities[0];
    entity->model = (BaseModel *) &font.vao;
    Vec3 entity_position_1 = newVec3(0.0, 0.0, 0.0);
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 1;

    entity = &renderer.entities[1];
    entity->model = &world_model;
    Vec3 entity_position_world = newVec3(0, 0, 0);
    entity->position = &entity_position_world;
    entity->active = 1;
    entity->scale = 1.0;

    // entity = &renderer.entities[1];
    // entity->model = &model;
    // Vec3 entity_position_2 = newVec3(0, 0, -20);
    // entity->position = &entity_position_2;
    // entity->active = 1;
    // entity->scale = 5.0;

    entity = &renderer.entities[0];
    entity->model = &suzanne;
    Vec3 entity_position_2 = newVec3(0, 1, 0);
    entity->position = &entity_position_2;
    entity->active = 1;
    entity->scale = 1.0;

    // entity = &renderer.entities[2];
    // entity->model = &suzanne;
    // Vec3 entity_position_3 = newVec3(10.0, 0.0, -50.0);
    // entity->position = &entity_position_3;
    // entity->active = 1;
    // entity->scale = 1.0;

    // entity = &renderer.entities[3];
    // entity->model = &tea_model;
    // Vec3 entity_position_4 = newVec3(10.0, 0.0, -30.0);
    // entity->position = &entity_position_4;
    // entity->active = 1;
    // entity->scale = 1.0;
    
    while (!glfwWindowShouldClose(ctx.window)) {
        printf("Entity SELETED: %d\n", entity_index);
        handle_input(&ctx, &renderer, &camera);

        render(&renderer, &camera);

        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera) {
    // glfwGetCursorPos(window, &xpos, &ypos);
    // double xpos, ypos;
    
    glfwGetWindowSize(ctx->window, &ctx->width, &ctx->height);
    double time = glfwGetTime();
    double second_per_frame = time - ctx->previous_time;
    ctx->previous_time = time;
    
    Entity *entity = &renderer->entities[entity_index];
    Entity *player = &renderer->entities[0];
    float d_player_move = 0.0;
    float rotation_factor = 0.1;

    if(
        glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_Q) == GLFW_PRESS
    ) {
        glfwSetWindowShouldClose(ctx->window, GL_TRUE);
    }

    if (glfwGetKey(ctx->window, GLFW_KEY_P ) == GLFW_PRESS){
        if ( (entity->fill & (1<<1)) == 0) {
            entity->fill |= (1<<1);
            entity->fill ^= 1;
        }
    } else {
        entity->fill &= ~(1<<1);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        increase_rotation(player, 0.0, rotation_factor * speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        increase_rotation(player, 0.0, -rotation_factor * speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        d_player_move += speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        d_player_move -= speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_N) == GLFW_PRESS ) {
        if (pulse_n == 0 ) {
            pulse_n = 1;
            entity_index++;
            if (entity_index > 3) {
                printf("%d", entity_index);
                entity_index = 0;
            }
        }
    }
    else {
        pulse_n = 0;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        printf("CONTROL PRESSED\n");
        if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {
            increase_rotation(entity, -speed, 0.0, 0.0);
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            printf("Button pressed\n");
            increase_rotation(entity, 0.0,speed, 0.0);
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            printf("Button pressed\n");
            increase_rotation(entity, speed, 0.0, 0.0);
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            printf("Button pressed\n");
            increase_rotation(entity, 0.0, -speed, 0.0);
        }
    }
    else {
        if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {

            distance_from_player -= speed;  
            if (distance_from_player <= 0.5) {
                distance_from_player = 0.5;
            }
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            distance_from_player += speed;
        }
    }


    if (glfwGetKey(ctx->window, GLFW_KEY_H) == GLFW_PRESS) {
        camera->yaw -= 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_L) == GLFW_PRESS) {
        camera->yaw += 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_J) == GLFW_PRESS) {
        camera->pitch -= 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_K) == GLFW_PRESS) {
        camera->pitch += 0.01;
    }

    player->position->x += d_player_move * sinf(player->rotation_y);
    player->position->z += d_player_move * cosf(player->rotation_y);

    float horizontal_distance = distance_from_player * cosf(camera->pitch);
    float vertical_distance = distance_from_player * sinf(camera->pitch);
    float theta = camera->yaw + player->rotation_y;

    camera->position.x = player->position->x - horizontal_distance*sinf(theta);
    camera->position.z = player->position->z - horizontal_distance*cos(theta);
    camera->position.y = player->position->y + vertical_distance;

    camera->centre = *player->position;

    float aspect_ratio = (float)ctx->width / (float)ctx->height;
    float font_aspect_ratio = renderer->font->base_x / renderer->font->base_y;
    font_buffer_reset(renderer->font, aspect_ratio);
    char msg[500];
    sprintf(msg, "Distance from player (zoom): %0.3f", distance_from_player);
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "pitch: %0.3f", camera->pitch);
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "yaw: %.3f", camera->yaw);
    font_buffer_push(renderer->font, msg);

    sprintf(
        msg, "aspect ratio: %.3f, font_aspect_ratio: %.3f",
        aspect_ratio, font_aspect_ratio
    );
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "FPS: %.3f", 1.0/second_per_frame);
    font_buffer_push(renderer->font, msg);
}

