#ifndef SHADER_H
#define SHADER_H

#include "common.h"

unsigned int shader_get_program();
unsigned int shader_get_program_2d();


void shader_push(unsigned int shader_program_id);
void shader_pop();
void shader_load_matrix(
    unsigned int shader_program_id,
    char *location,
    Mat4 *matrix
);


#endif  // SHADER_H
