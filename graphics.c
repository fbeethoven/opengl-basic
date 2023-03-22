#include "graphics.h"
#include "shader.h"


G_Object *graphics_new_object() {
    glEnable(GL_DEPTH_TEST);
    G_Object *new_object = (G_Object*) malloc(sizeof(G_Object));
    memset(new_object, 0, sizeof(G_Object));


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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return new_object;
}


void graphics_free_object(G_Object *object){
    glDeleteVertexArrays(1, &object->vao);
    glDeleteBuffers(1, &object->vbo);
    glDeleteBuffers(1, &object->ibo);
    glDeleteProgram(object->shader_program_id);
}
