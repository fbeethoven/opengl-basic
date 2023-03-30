
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"
#include "graphics.h"
#include "utils/file_handler.h"


#define BAD_COORDS 1

float speed;
int entity_index;
int pulse_n;


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);


typedef struct Mesh {
    Vec3 *vertices;
    Vec2 *uvs;
    unsigned int *indices;

    int vertices_len;
    int uvs_len;
    int indices_len;
} Mesh;


void draw_quad(Mesh *mesh, Vec3 position) {
    float width = 5.0;
    int v_c = mesh->vertices_len;
    int u_c = mesh->uvs_len;
    int i_c = mesh->indices_len;
    mesh->vertices[v_c] = newVec3(position.x - width, 0, position.z - width);
    mesh->vertices[v_c + 1] = newVec3(position.x - width, 0, position.z + width);
    mesh->vertices[v_c + 2] = newVec3(position.x + width, 0, position.z - width);
    mesh->vertices[v_c + 3] = newVec3(position.x + width, 0, position.z + width);

    mesh->uvs[u_c] = newVec2(0.0, 0.0);
    mesh->uvs[u_c + 1] = newVec2(0.0, 1.0);
    mesh->uvs[u_c + 2] = newVec2(1.0, 0.0);
    mesh->uvs[u_c + 3] = newVec2(1.0, 1.0);

    mesh->indices[i_c] = v_c;
    mesh->indices[i_c + 1] = v_c + 1;
    mesh->indices[i_c + 2] = v_c + 2;

    mesh->indices[i_c + 3] = v_c + 1;
    mesh->indices[i_c + 4] = v_c + 3;
    mesh->indices[i_c + 5] = v_c + 2;


    mesh->vertices_len += 4;
    mesh->uvs_len += 4;
    mesh->indices_len += 6;
}


int main() {
    speed = 0.05;
    entity_index = 0;
    pulse_n = 0;

    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(5.0, 5.0, 0.0);
    camera.pitch = 0.1745;
    camera.yaw = 0.0;

    float tile_size = 10.0;
    int tiles_per_chunk = 10;
    int world = 100;

    Vec3 vertices[64*64];
    Vec2 uvs[64*64];
    unsigned int indices[3*64*64];

    Mesh mesh = {0};
    mesh.vertices = vertices;
    mesh.uvs = uvs;
    mesh.indices = indices;

    for (int z=-tiles_per_chunk; z<tiles_per_chunk; z++) {
        for (int x=-tiles_per_chunk; x<tiles_per_chunk; x++) {
            draw_quad(&mesh, newVec3(x * tile_size, 0, z * tile_size));
        }
    }



    float text_coord[] = {
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f
		};

    BaseModel world_marble = {0};
    load_data_to_model(
        &world_marble, (float *) mesh.vertices, mesh.indices,
        3*sizeof(float)*mesh.vertices_len, sizeof(unsigned int)*mesh.indices_len
    );
    load_texture_to_model(
        &world_marble,
        "assets/textures/marble-floor.jpg",
        (float *)mesh.uvs, 
        2 * sizeof(float) * mesh.uvs_len
    );
    world_marble.vertex_count = mesh.indices_len;

    BaseModel world_wall = {0};
    load_data_to_model(
        &world_wall, (float *) mesh.vertices, mesh.indices,
        3*sizeof(float)*mesh.vertices_len, sizeof(unsigned int)*mesh.indices_len
    );
    load_texture_to_model(
        &world_wall,
        "assets/textures/wall.jpg",
        (float *)mesh.uvs, 
        2 * sizeof(float) * mesh.uvs_len
    );
    world_wall.vertex_count = mesh.indices_len;
    BaseModel world_wood = {0};
    load_data_to_model(
        &world_wood, (float *) mesh.vertices, mesh.indices,
        3*sizeof(float)*mesh.vertices_len, sizeof(unsigned int)*mesh.indices_len
    );
    load_texture_to_model(
        &world_wood,
        "assets/textures/wood-floor.jpg",
        (float *)mesh.uvs, 
        2 * sizeof(float) * mesh.uvs_len
    );
    world_wood.vertex_count = mesh.indices_len;

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
    entity->model = &world_marble;
    Vec3 entity_position_1 = newVec3(0, 0, 0);
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 1.0;

    entity = &renderer.entities[1];
    entity->model = &world_wall;
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 1.0;

    entity = &renderer.entities[2];
    entity->model = &world_wood;
    entity->position = &entity_position_1;
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
        printf("Button pressed\n");
        camera_move(camera, speed, 0.0, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, 0.0, 0.0, speed);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, -speed, 0, 0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, 0.0, 0.0, -speed);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_R) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, 0.0, -speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_F) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, 0.0, speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_N) == GLFW_PRESS ) {
        if (pulse_n == 0 ) {
            pulse_n = 1;

            entity->active = 0;

            entity_index++;

            if (entity_index > 2) {
                printf("%d", entity_index);
                entity_index = 0;
            }

            entity = &renderer->entities[entity_index];
            entity->active = 1;
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
    else if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, 0.0, speed);
    }
    else if (glfwGetKey(ctx->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, -speed, 0.0, 0.0);
    }
    else if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, 0.0, -speed);
    }
    else if (glfwGetKey(ctx->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, speed, 0.0, 0.0);
    }
    else if (glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera_move(camera, 0.0, speed, 0.0);
    }
    else if (glfwGetKey(ctx->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera_move(camera, 0.0, -speed, 0.0);
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

    printf(
        "CAMERA: %f %f %f\n",
        camera->position.x,
        camera->position.y,
        camera->position.z
    );
    printf("pitch: %f\n", camera->pitch);
    printf("yaw: %f\n", camera->yaw);
}


