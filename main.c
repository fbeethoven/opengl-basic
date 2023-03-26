/* Modified from: https://learnopengl.com/Getting-started/Hello-Window */
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

    camera.position = newVec3(5.0, 0.0, 0.0);
    camera.pitch = 0.1745;
    camera.yaw = 0.0;

    float vertices[] = {
        -0.5f,0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,0.5f,-0.5f,

         -0.5f,0.5f,0.5f,
         -0.5f,-0.5f,0.5f,
         0.5f,-0.5f,0.5f,
         0.5f,0.5f,0.5f,

         0.5f,0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,0.5f,
         0.5f,0.5f,0.5f,

         -0.5f,0.5f,-0.5f,
         -0.5f,-0.5f,-0.5f,
         -0.5f,-0.5f,0.5f,
         -0.5f,0.5f,0.5f,

         -0.5f,0.5f,0.5f,
         -0.5f,0.5f,-0.5f,
         0.5f,0.5f,-0.5f,
         0.5f,0.5f,0.5f,

         -0.5f,-0.5f,0.5f,
         -0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,-0.5f,
         0.5f,-0.5f,0.5f
		};

    unsigned int indices[] = {
        0,1,3,
        3,1,2,
        4,5,7,
        7,5,6,
        8,9,11,
        11,9,10,
        12,13,15,
        15,13,14,
        16,17,19,
        19,17,18,
        20,21,23,
        23,21,22
    };

    BaseModel model = {0};
    load_data_to_model(
        &model, vertices, indices,
        sizeof(vertices), sizeof(indices)
    );
    model.vertex_count = sizeof(indices)/sizeof(indices[0]);


    BaseModel tea_model = {0};
    IntermediateModel tmp = {0};
    parse_obj_file_simple("assets/models/utah_teapot.obj", &tmp);
    load_data_to_model(
        &tea_model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    tea_model.vertex_count = tmp.indices_count;


    BaseModel suzenne = {0};
    IntermediateModel suzenne_data = {0};
    parse_obj_file("assets/models/suzanne.obj", &suzenne_data);
    load_data_to_model(
        &suzenne, suzenne_data.vertices, suzenne_data.indices,
        suzenne_data.vertices_count* sizeof(float),
        suzenne_data.indices_count * sizeof(unsigned int)
    );
    suzenne.vertex_count = suzenne_data.indices_count;

    Entity *entity = &renderer.entities[0];
    entity->model = &model;
    Vec3 entity_position_1 = newVec3(20, 0, -50);
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 3.0;

    entity = &renderer.entities[1];
    entity->model = &model;
    Vec3 entity_position_2 = newVec3(0, 0, -20);
    entity->position = &entity_position_2;
    entity->active = 1;
    entity->scale = 5.0;

    entity = &renderer.entities[2];
    entity->model = &suzenne;
    Vec3 entity_position_3 = newVec3(10.0, 0.0, -50.0);
    entity->position = &entity_position_3;
    entity->active = 1;
    entity->scale = 1.0;

    entity = &renderer.entities[3];
    entity->model = &tea_model;
    Vec3 entity_position_4 = newVec3(10.0, 0.0, -30.0);
    entity->position = &entity_position_4;
    entity->active = 1;
    entity->scale = 1.0;


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
        camera_move(camera, 0.0, 0.0, -speed);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, -speed, 0, 0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        printf("Button pressed\n");
        camera_move(camera, 0.0, 0.0, speed);
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
        printf("Button pressed\n");
        increase_position(entity, 0.0, speed, 0.0);
    }
    else if (glfwGetKey(ctx->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, -speed, 0.0);
    }




    if (glfwGetKey(ctx->window, GLFW_KEY_J) == GLFW_PRESS) {
        entity->scale -= 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_K) == GLFW_PRESS) {
        entity->scale += 0.01;
    }

    printf("scale: %f\n", entity->scale);
    printf("pitch: %f\n", camera->pitch);
    printf("yaw: %f\n", camera->yaw);
}


