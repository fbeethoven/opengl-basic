/* Modified from: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION

#include "common.h"
#include "graphics.h"


void move_cube(float vertices[], int n, Vec3 *dir, Mat4 *cube_mat) {
//    for (int i=0; i<n; i++) {
//        vertices[i*3 + 0] += dir->x;
//        vertices[i*3 + 1] += dir->y;
//        vertices[i*3 + 2] += dir->z;
//    }
    Mat4 transpose = Mat4I();
    Mat4Translate(&transpose, dir);
    Mat4 new_transform = Mat4Mult(&transpose, cube_mat);

    Mat4Copy(cube_mat, &new_transform);
}


void process_input(GLFWwindow *window, float vertices[], int *triangles, Mat4 *cube_mat){
    double xpos, ypos;
    float speed = 0.05;

//    printf("DEBUG VERTICES\n");
//    printf("x: %f, y: %f z: %f\n", vertices[0], vertices[1], vertices[2]);
//
//    glfwGetCursorPos(window, &xpos, &ypos);
//    printf("x: %f, y: %f\n", xpos, ypos);

//    printf("DEBUG CUBE CAMERA\n");
//    printf("%f %f %f %f\n", cube->camera_controler[0][0], cube->camera_controler[0][1], cube->camera_controler[0][2], cube->camera_controler[0][3]);
//    printf("%f %f %f %f\n", cube->camera_controler[1][0], cube->camera_controler[1][1], cube->camera_controler[1][2], cube->camera_controler[1][3]);
//    printf("%f %f %f %f\n", cube->camera_controler[2][0], cube->camera_controler[2][1], cube->camera_controler[2][2], cube->camera_controler[2][3]);
//    printf("%f %f %f %f\n", cube->camera_controler[3][0], cube->camera_controler[3][1], cube->camera_controler[3][2], cube->camera_controler[3][3]);

    int shift = 0;

    if(
        glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS
    ) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_P ) == GLFW_PRESS){
        if ( (*triangles & (1<<1)) == 0) {
            *triangles |= (1<<1);
            *triangles ^= 1;
        }
    } else {
        *triangles &= ~(1<<1);
    }


    if (
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS
    ){
        printf("Going BACKWARDS!\n");
        Vec3 dir = { .x = 0.0, .y = 0.0, .z= -speed };
        move_cube(vertices, 36, &dir, cube_mat);
        shift = 1;
    } else {
        shift = 0;
    }
    if (
        glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_K ) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS
    ){
        printf("Pressing UP\n");
        Vec3 dir = { .x = 0.0, .y = +speed, .z=0.0 };
        move_cube(vertices, 36, &dir, cube_mat);
    }
    if (
        shift == 0 &&
        glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS
    ){
        printf("Going FORWARD!\n");
        Vec3 dir = { .x = 0.0, .y = 0.0, .z= speed };
        move_cube(vertices, 36, &dir, cube_mat);
    }

    if (
        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
    ){
        printf("Pressing DOWN\n");
        Vec3 dir = { .x = 0.0, .y = -speed, .z=0.0 };
        move_cube(vertices, 36, &dir, cube_mat);
    }
    if (
        glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_L ) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS
    ){
        printf("Pressing RIGHT\n");
        Vec3 dir = { .x = speed, .y = 0.0, .z=0.0 };
        move_cube(vertices, 36, &dir, cube_mat);
    }
    if (
        glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
    ){
        printf("Pressing LEFT\n");
        Vec3 dir = { .x = -speed, .y = 0.0, .z=0.0 };
        move_cube(vertices, 36, &dir, cube_mat);
    }

}


int main() {
    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    float vertices[] = {
        -0.5f,  0.5f, 0.0f,  // top left
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f,  // bottom right
         0.5f,  0.5f, 0.0f,  // top right

        -0.5f,  0.5f, 0.5f,  // top left
         0.5f,  0.5f, 0.5f,  // top right
         0.5f, -0.5f, 0.5f,  // bottom right
        -0.5f, -0.5f, 0.5f  // bottom left
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,

        1, 4, 7,
        7, 1, 0,

        3, 2, 5,
        3, 5, 6,

        1, 4, 5,
        1, 5, 2
    };


    G_Object* cube = graphics_new_object();
    Mat4 cube_mat = Mat4I();


    Vec3 a = newVec3(-0.75, -0.75, 0.0);
    Vec3 b = newVec3(-0.80, -0.75, 0.0);
    Vec3 c = newVec3(-0.80, -0.80, 0.0);
    Vec3 d = newVec3(-0.75, -0.80, 0.0);

    G_Object *rect = graphics_new_rect(&ctx, &a, &b, &c, &d);
    int triangles = 0;
    while (!glfwWindowShouldClose(ctx.window)) {
        process_input(ctx.window, vertices, &triangles, &cube_mat);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        graphics_render_rect(&ctx, rect);

		glBindVertexArray(cube->vao);
        glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glUseProgram(cube->shader_program_id);

        Mat4 transform = Mat4Mult((Mat4 *)cube->camera_controler, &cube_mat);

        printf("TRANSFORM MATRIX\n");
        printf("%f, %f, %f, %f\n", transform.T[0][0], transform.T[0][1], transform.T[0][2], transform.T[0][3]);
        printf("%f, %f, %f, %f\n", transform.T[1][0], transform.T[1][1], transform.T[1][2], transform.T[1][3]);
        printf("%f, %f, %f, %f\n", transform.T[2][0], transform.T[2][1], transform.T[2][2], transform.T[2][3]);
        printf("%f, %f, %f, %f\n", transform.T[3][0], transform.T[3][1], transform.T[3][2], transform.T[3][3]);


        int uniform_location = glGetUniformLocation(cube->shader_program_id, "u_MVP");
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &cube->camera_controler[0][0]);
//        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &transform.T[0][0]);
        assert(uniform_location >= 0);


        uniform_location = glGetUniformLocation(cube->shader_program_id, "u_transform");
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &cube_mat.T[0][0]);
        assert(uniform_location >= 0);

        float u_color[] = {1.0, 0.0, 0.0, 1.0};

        uniform_location = glGetUniformLocation(cube->shader_program_id, "u_color");
        glUniform4fv(uniform_location, 1, &u_color[0]);
        assert(uniform_location >= 0);
        printf("Uniform color location: %d\n", uniform_location);


        if ( (triangles & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        glDrawElements(GL_TRIANGLES, sizeof(indices)/3, GL_UNSIGNED_INT, 0);

        glUseProgram(0);
        glBindVertexArray(0);


        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    graphics_free_object(cube);
    graphics_free_object(rect);

    glfwTerminate();
    return 0;
}
