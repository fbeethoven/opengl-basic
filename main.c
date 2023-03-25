/* Modified from: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"
#include "graphics.h"

#define BAD_COORDS 1

// void move_cube(float vertices[], int n, Vec3 *dir, Mat4 *cube_mat) {
// 
//     if (BAD_COORDS) {
//         for (int i=0; i<4; i++) {
//             vertices[i*3 + 0] += dir->x;
//             vertices[i*3 + 1] += dir->y;
//             vertices[i*3 + 2] += dir->z;
//         }
//     }
//     else {
//         Mat4 transpose = Mat4I();
//         Mat4Translate(&transpose, dir);
//         Mat4 new_transform = Mat4Mult(&transpose, cube_mat);
// 
//         Mat4Copy(cube_mat, &new_transform);
//     }
// }
// 
// 
// void process_input(GLFWwindow *window, float vertices[], int *triangles, Mat4 *cube_mat){
//     double xpos, ypos;
//     float speed = 0.05;
// 
// //    printf("DEBUG VERTICES\n");
// //    printf("x: %f, y: %f z: %f\n", vertices[0], vertices[1], vertices[2]);
// //
// //    glfwGetCursorPos(window, &xpos, &ypos);
// //    printf("x: %f, y: %f\n", xpos, ypos);
// 
// //    printf("DEBUG CUBE CAMERA\n");
// //    printf("%f %f %f %f\n", cube->camera_controler[0][0], cube->camera_controler[0][1], cube->camera_controler[0][2], cube->camera_controler[0][3]);
// //    printf("%f %f %f %f\n", cube->camera_controler[1][0], cube->camera_controler[1][1], cube->camera_controler[1][2], cube->camera_controler[1][3]);
// //    printf("%f %f %f %f\n", cube->camera_controler[2][0], cube->camera_controler[2][1], cube->camera_controler[2][2], cube->camera_controler[2][3]);
// //    printf("%f %f %f %f\n", cube->camera_controler[3][0], cube->camera_controler[3][1], cube->camera_controler[3][2], cube->camera_controler[3][3]);
// 
//     int shift = 0;
// 
//     if(
//         glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
//         glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS
//     ) {
//         glfwSetWindowShouldClose(window, GL_TRUE);
//     }
// 
//     if (glfwGetKey(window, GLFW_KEY_P ) == GLFW_PRESS){
//         if ( (*triangles & (1<<1)) == 0) {
//             *triangles |= (1<<1);
//             *triangles ^= 1;
//         }
//     } else {
//         *triangles &= ~(1<<1);
//     }
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


int main() {
    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(0, 5, 0);
    // camera.pitch = 10;
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
    Entity entity = renderer.entities[0];
    entity.model = &model;
    Vec3 entity_position = newVec3(0, 0, 0);
    entity.position = &entity_position;
    entity.active = 1;

    while (!glfwWindowShouldClose(ctx.window)) {
        float speed = 0.05;
        if(
            glfwGetKey(ctx.window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(ctx.window, GLFW_KEY_Q) == GLFW_PRESS
        ) {
            glfwSetWindowShouldClose(ctx.window, GL_TRUE);
        }
        if (glfwGetKey(ctx.window, GLFW_KEY_A) == GLFW_PRESS) {
            camera_move(&camera, speed, 0, 0);
        }
        

        // process_input(ctx.window, vertices, &triangles, &cube_mat);
        render(&renderer, &camera);

        // if ( (triangles & 1) == 0) {
        //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // }
        // else {
        //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // }



        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
