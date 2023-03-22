/* Modified from: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION

#include "common.h"
#include "graphics.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


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


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    float top = 1.0;
    float right = 1.0;
    float bottom = -1.0;
    float left = -1.0;
    float near = -1.0;
    float far = 1.0;
    float mat_ortho[][4] = {
        {-(2*near)/ (right - left),      0                , (right + left)/(right - left),            0},
        {            0           ,-(2*near)/(top - bottom), (top + bottom)/(top - bottom),            0},
        {            0           ,      0                , (far + near)/(far - near)   ,-(2*far*near)/(far - near)},
        {            0           ,      0                ,             0.0              ,            1}
    };

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
//    unsigned int VBO, VAO, EBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//    glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_DYNAMIC_DRAW);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//
//
//    unsigned int shader_program_id = shader_get_program();

    G_Object* cube = graphics_new_object();
    int triangles = 0;
    while (!glfwWindowShouldClose(window)) {
        process_input(window, vertices, &triangles);

//        glDepthFunc(GL_LESS);
//        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glUseProgram(cube->shader_program_id);


        int uniform_location = glGetUniformLocation(cube->shader_program_id, "u_MVP");
        glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &mat_ortho[0][0]);
        printf("uniform_location: %d\n", uniform_location);

		glBindVertexArray(cube->vao);

        if ( (triangles & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        glDrawElements(GL_TRIANGLES, sizeof(indices)/3, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
//    glDeleteProgram(shader_program_id);
    graphics_free_object(cube);

    glfwTerminate();
    return 0;
}
