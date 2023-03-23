#include "graphics.h"
#include "shader.h"
#include "image.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


G_Object *graphics_new_object() {
    glEnable(GL_DEPTH_TEST);
    G_Object *new_object = (G_Object*) malloc(sizeof(G_Object));
    memset(new_object, 0, sizeof(G_Object));

    float top = 1.0;
    float right = 1.0;
    float bottom = -1.0;
    float left = -1.0;
    float near = -1.0;
    float far = 1.0;

    new_object->camera_controler[0][0] = (2*near)/ (right - left);
    new_object->camera_controler[0][1] = 0;
    new_object->camera_controler[0][2] = (right + left)/(right - left);
    new_object->camera_controler[0][3] = 0;

    new_object->camera_controler[1][0] = 0;
    new_object->camera_controler[1][1] = (2*near)/(top - bottom);
    new_object->camera_controler[1][2] = (top + bottom)/(top - bottom);
    new_object->camera_controler[1][3] = 0;

    new_object->camera_controler[2][0] = 0;
    new_object->camera_controler[2][1] = 0;
    new_object->camera_controler[2][2] = -(far + near)/(far - near);
    new_object->camera_controler[2][3] = -(2*far*near)/(far - near);

    new_object->camera_controler[3][0] = 0;
    new_object->camera_controler[3][1] = 0;
    new_object->camera_controler[3][2] = -1;
    new_object->camera_controler[3][3] = 1;


    float vertices[] = {
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  // top left
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // bottom right
    0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // top right

    -0.5f,  0.5f, 0.5f, 0.0f, 1.0f,  // top left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom left
    0.5f, -0.5f, 0.5f,  1.0f, 0.0f,  // bottom right
    0.5f,  0.5f, 0.5f,  1.0f, 1.0f  // top right
    };

    unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0

//    2, 6, 7,
//    7, 3, 2

//    3, 2, 5,
//    3, 5, 6,
//
//    1, 4, 5,
//    1, 5, 2
    };

    new_object->shader_program_id = shader_get_program();

    // glCreateVertexArrays(1, &new_object->vao);
    glGenVertexArrays(1, &new_object->vao);
    glBindVertexArray(new_object->vao);

    glGenBuffers(1, &new_object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, new_object->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_DYNAMIC_DRAW);


    glGenBuffers(1, &new_object->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_object->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    glGenTextures(1, &new_object->texture_id);
    glBindTexture(GL_TEXTURE_2D, new_object->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    char *image_path = "texture920x613.png";
    char *image_path = "wall.jpg";
    Image *test = image_load(image_path);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, test->width, test->height, 0, GL_RGB, GL_UNSIGNED_BYTE, test->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    image_free(test);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return new_object;
}

void graphics_render_cube(G_Object *object) {

}



void graphics_free_object(G_Object *object){
    glDeleteVertexArrays(1, &object->vao);
    glDeleteBuffers(1, &object->vbo);
    glDeleteBuffers(1, &object->ibo);
    glDeleteProgram(object->shader_program_id);
}


int graphics_init(GraphicsContext *ctx) {
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
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
    ctx->window = window;
    return 0;
}


void graphics_render_rect(GraphicsContext *ctx, G_Object *object) {
    glBindVertexArray(object->vao);
    // glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
    glUseProgram(object->shader_program_id);
    glDrawElements(GL_TRIANGLES, 2, GL_UNSIGNED_INT, 0);
    glUseProgram(0);
    glBindVertexArray(0);
}

G_Object *graphics_new_rect(
    GraphicsContext *ctx,
    Vec3 *topright, Vec3 *topleft, Vec3 *botleft, Vec3 *botright
) {
    glEnable(GL_DEPTH_TEST);
    G_Object *new_object = (G_Object*) malloc(sizeof(G_Object));
    memset(new_object, 0, sizeof(G_Object));


    printf("RENDER DEBUG\n");
    printf("x: %f, y: %f, z: %f\n", topright->x, topright->y, topright->z);

    float vertices[] = {
        topright->x, topright->y, topright->z,
        topleft->x, topleft->y, topleft->z,
        botleft->x, botleft->y, botleft->z,
        botright->x, botright->y, botright->z
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    new_object->shader_program_id = shader_get_program_2d();

    // glCreateVertexArrays(1, &new_object->vao);
    glGenVertexArrays(1, &new_object->vao);
    glBindVertexArray(new_object->vao);

    glGenBuffers(1, &new_object->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, new_object->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glGenBuffers(1, &new_object->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_object->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return new_object;
}
