/* Modified from: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION

#include "common.h"
#include "graphics.h"


void move_cube(float vertices[], int n, Vec3 *dir) {
    for (int i=0; i<n; i++) {
        vertices[i*3 + 0] += dir->x;
        vertices[i*3 + 1] += dir->y;
        vertices[i*3 + 2] += dir->z;
    }
}


void process_input(GLFWwindow *window, float vertices[], int *triangles){
    double xpos, ypos;
    float speed = 0.05;

    printf("DEBUG VERTICES\n");
    printf("x: %f, y: %f z: %f\n", vertices[0], vertices[1], vertices[2]);

    glfwGetCursorPos(window, &xpos, &ypos);
    printf("x: %f, y: %f\n", xpos, ypos);
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
        move_cube(vertices, 36, &dir);
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
        move_cube(vertices, 36, &dir);
    }
    if (
        shift == 0 &&
        glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS
    ){
        printf("Going FORWARD!\n");
        Vec3 dir = { .x = 0.0, .y = 0.0, .z= speed };
        move_cube(vertices, 36, &dir);
    }

    if (
        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
    ){
        printf("Pressing DOWN\n");
        Vec3 dir = { .x = 0.0, .y = -speed, .z=0.0 };
        move_cube(vertices, 36, &dir);
    }
    if (
        glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_L ) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS
    ){
        printf("Pressing RIGHT\n");
        Vec3 dir = { .x = speed, .y = 0.0, .z=0.0 };
        move_cube(vertices, 36, &dir);
    }
    if (
        glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS
    ){
        printf("Pressing LEFT\n");
        Vec3 dir = { .x = -speed, .y = 0.0, .z=0.0 };
        move_cube(vertices, 36, &dir);
    }

}


int main() {
    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  // top left
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right

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

    Vec3 a = newVec3(-0.75, -0.75, 0.0);
    Vec3 b = newVec3(-0.80, -0.75, 0.0);
    Vec3 c = newVec3(-0.80, -0.80, 0.0);
    Vec3 d = newVec3(-0.75, -0.80, 0.0);

    G_Object *rect = graphics_new_rect(&ctx, &a, &b, &c, &d);
    int triangles = 0;
    while (!glfwWindowShouldClose(ctx.window)) {
        process_input(ctx.window, vertices, &triangles);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(cube->vao);
        glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glUseProgram(cube->shader_program_id);


        int uniform_location = glGetUniformLocation(cube->shader_program_id, "u_MVP");
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &cube->camera_controler[0][0]);
        printf("uniform_location: %d\n", uniform_location);


        if ( (triangles & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        glDrawElements(GL_TRIANGLES, sizeof(indices)/3, GL_UNSIGNED_INT, 0);


        graphics_render_rect(&ctx, rect);



        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    graphics_free_object(cube);

    glfwTerminate();
    return 0;
}
