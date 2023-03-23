#ifndef COMMON_H
#define COMMON_H


#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "stb_image.h"


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

typedef struct Mat4 {
    float T[4][4];
} Mat4;

Mat4 Mat4Diag(float x, float y, float z, float w);
Mat4 Mat4I();
Mat4 Mat4Add(Mat4 *A, Mat4 *B);
void Mat4Copy(Mat4 *A, Mat4 *B);
Mat4 Mat4Mult(Mat4 *A, Mat4 *B);
void Mat4Scale(Mat4 *A, float c);
void Mat4Translate(Mat4 *A, Vec3 *dir);
Mat4 Mat4Transpose(Mat4 *A);

Vec3 newVec3(float x, float y, float z);

#endif  // COMMON_H
