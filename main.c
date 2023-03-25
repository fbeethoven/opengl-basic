/* Modified from: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"
#include "graphics.h"

#define BAD_COORDS 1
float speed;

void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);



int main() {
    speed = 0.05;

    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(0.0, 5.0, 0.0);
    // camera.pitch = 10;
    camera.pitch = 0.1745;
    camera.yaw = 0.0;

    float vertices[] = {
        -0.5f,0.5f,-0.5f,	
        -0.5f,-0.5f,-0.5f,	
        0.5f,-0.5f,-0.5f,	
        0.5f,0.5f,-0.5f

//         -0.5f,0.5f,0.5f,	
//         -0.5f,-0.5f,0.5f,	
//         0.5f,-0.5f,0.5f,	
//         0.5f,0.5f,0.5f,
// 
//         0.5f,0.5f,-0.5f,	
//         0.5f,-0.5f,-0.5f,	
//         0.5f,-0.5f,0.5f,	
//         0.5f,0.5f,0.5f,
// 
//         -0.5f,0.5f,-0.5f,	
//         -0.5f,-0.5f,-0.5f,	
//         -0.5f,-0.5f,0.5f,	
//         -0.5f,0.5f,0.5f,
// 
//         -0.5f,0.5f,0.5f,
//         -0.5f,0.5f,-0.5f,
//         0.5f,0.5f,-0.5f,
//         0.5f,0.5f,0.5f,
// 
//         -0.5f,-0.5f,0.5f,
//         -0.5f,-0.5f,-0.5f,
//         0.5f,-0.5f,-0.5f,
//         0.5f,-0.5f,0.5f
		};

    unsigned int indices[] = {
        0,1,3,	
        // 3,1,2
//        4,5,7,
//        7,5,6,
//        8,9,11,
//        11,9,10,
//        12,13,15,
//        15,13,14,	
//        16,17,19,
//        19,17,18,
//        20,21,23,
//        23,21,22
    };

    BaseModel model = {0};

    load_data_to_model(
        &model, vertices, indices,
        sizeof(vertices), sizeof(indices)
    );
    model.vertex_count = sizeof(indices)/sizeof(indices[0]);
    Entity *entity = &renderer.entities[0];
    entity->model = &model;
    Vec3 entity_position = newVec3(0, 0, 0);
    entity->position = &entity_position;
    entity->active = 1;
    entity->scale = 5.0;

    while (!glfwWindowShouldClose(ctx.window)) {
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
    Entity *entity = &renderer->entities[0];
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
    if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, 0.0, speed);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, -speed, 0.0, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, 0.0, -speed);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, speed, 0.0, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        printf("Button pressed\n");
        increase_position(entity, 0.0, -speed, 0.0);
    }
}
// 
// 
// 
//     if (
//         glfwGetKey(window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS &&
//         glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS
//     ){
//         printf("Going BACKWARDS!\n");
//         Vec3 dir = { .x = 0.0, .y = 0.0, .z= -speed };
//         move_cube(vertices, 36, &dir, cube_mat);
//         shift = 1;
//     } else {
//         shift = 0;
//     }
//     if (
//         glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_K ) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS
//     ){
//         printf("Pressing UP\n");
//         Vec3 dir = { .x = 0.0, .y = +speed, .z=0.0 };
//         move_cube(vertices, 36, &dir, cube_mat);
//     }
//     if (
//         shift == 0 &&
//         glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS
//     ){
//         printf("Going FORWARD!\n");
//         Vec3 dir = { .x = 0.0, .y = 0.0, .z= speed };
//         move_cube(vertices, 36, &dir, cube_mat);
//     }
// 
//     if (
//         glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
//     ){
//         printf("Pressing DOWN\n");
//         Vec3 dir = { .x = 0.0, .y = -speed, .z=0.0 };
//         move_cube(vertices, 36, &dir, cube_mat);
//     }
//     if (
//         glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_L ) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS
//     ){
//         printf("Pressing RIGHT\n");
//         Vec3 dir = { .x = speed, .y = 0.0, .z=0.0 };
//         move_cube(vertices, 36, &dir, cube_mat);
//     }
//     if (
//         glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
//     ){
//         printf("Pressing LEFT\n");
//         Vec3 dir = { .x = -speed, .y = 0.0, .z=0.0 };
//         move_cube(vertices, 36, &dir, cube_mat);
//     }
// 
// }




