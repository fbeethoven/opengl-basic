#ifndef SHADER_H
#define SHADER_H

#include "common.h"


typedef struct Light {
    Vec3 position;
    Vec3 color;
} Light;


unsigned int shader_get_program();
unsigned int shader_get_program_2d();
unsigned int shader_get_program_general(
    char *vertex_shader_path, char* fragment_shader_path
);


void shader_push(unsigned int shader_program_id);
void shader_pop();
void shader_load_float(
    unsigned int shader_program_id,
    char *location,
    float x
);
void shader_load_vec2(
    unsigned int shader_program_id,
    char *location,
    Vec2 *vec
);
void shader_load_vec3(
    unsigned int shader_program_id,
    char *location,
    Vec3 *vec
);
void shader_load_vec4(
    unsigned int shader_program_id,
    char *location,
    Vec4 *vec
);
void shader_load_matrix(
    unsigned int shader_program_id,
    char *location,
    Mat4 *matrix
);

void shader_load_light(unsigned int shader_program_id, Light *light);


#endif  // SHADER_H
