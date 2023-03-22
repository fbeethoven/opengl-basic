#ifndef COMMON_H
#define COMMON_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

typedef struct Vec4 {
    float x;
    float y;
    float z;
    float w;
} Vec4;


Vec3 newVec3(float x, float y, float z);

#endif  // COMMON_H
