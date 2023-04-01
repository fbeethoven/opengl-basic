/* Modified fom: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"
#include "graphics.h"
#include "mesh.h"
#include "utils/file_handler.h"


#define BAD_COORDS 1

float speed;
int entity_index;
int pulse_n;

float distance_from_player;


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);


int main() {
    speed = 0.2;
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


    // mesh usage example
    Mesh mesh = {0};

    Vec3 mesh_vertices[64*64];
    Vec2 uvs[64*64];
    unsigned int mesh_indices[3*64*64];

    mesh.vertices = mesh_vertices;
    mesh.uvs = uvs;
    mesh.indices = mesh_indices;
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
    world_model.vertex_count = mesh.indices_len;


    float vertices[] = {
        -0.5f,0.5f, 0.0f,
        -0.5f,-0.5f, 0.0f,
        0.5f,-0.5f, 0.0f,
        0.5f,0.5f, 0.0f
		};

    float text_coord[] = {
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f
		};

    unsigned int indices[] = {
        0,1,3,
        3,1,2
    };

    BaseModel rect = {0};
    load_data_to_model(
        &rect, vertices, indices,
        sizeof(vertices), sizeof(indices)
    );
    load_texture_to_model(
        &rect, "assets/fonts/charmap-oldschool_white.png", text_coord, 
        // &rect, "assets/textures/marble-floor.jpg", text_coord, 
        sizeof(text_coord)
    );
    rect.vertex_count = sizeof(indices)/sizeof(indices[0]);

    BaseModel model = {0};
    IntermediateModel cube_data = {0};
    parse_obj_file_simple("assets/models/cube.obj", &cube_data);
    load_data_to_model(
        &model, cube_data.vertices, cube_data.indices,
        cube_data.vertices_count * sizeof(float),
        cube_data.indices_count * sizeof(unsigned int)
    );
    model.vertex_count = cube_data.indices_count;


    BaseModel tea_model = {0};
    IntermediateModel tmp = {0};
    parse_obj_file_simple("assets/models/utah_teapot.obj", &tmp);
    load_data_to_model(
        &tea_model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    tea_model.vertex_count = tmp.indices_count;


    BaseModel suzanne = {0};
    IntermediateModel suzanne_data = {0};
    parse_obj_file("assets/models/suzanne.obj", &suzanne_data);
    load_data_to_model(
        &suzanne, suzanne_data.vertices, suzanne_data.indices,
        suzanne_data.vertices_count* sizeof(float),
        suzanne_data.indices_count * sizeof(unsigned int)
    );
    suzanne.vertex_count = suzanne_data.indices_count;

    Entity *entity = &renderer.entities[0];
    entity->model = &rect;
    Vec3 entity_position_1 = newVec3(5, 0, -5);
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 3.0;

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
        if ( (renderer->fill & (1<<1)) == 0) {
            renderer->fill |= (1<<1);
            renderer->fill ^= 1;
        }
    } else {
        renderer->fill &= ~(1<<1);
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
            // d_camera_move += speed;

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

    printf("scale: %f\n", entity->scale);
    printf("pitch: %f\n", camera->pitch);
    printf("yaw: %f\n", camera->yaw);
}

