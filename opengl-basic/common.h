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


typedef uint8_t U8;
typedef int8_t I8;
typedef uint32_t U32;
typedef int32_t I32;
typedef uint64_t U64;
typedef int64_t I64;


typedef struct Vec2 {
    float x;
    float y;
} Vec2;


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

typedef struct Quat {
    float x;
    float y;
    float z;
    float w;
} Quat;


typedef struct Mat4 {
    float m00;
    float m01;
    float m02;
    float m03;

    float m10;
    float m11;
    float m12;
    float m13;

    float m20;
    float m21;
    float m22;
    float m23;

    float m30;
    float m31;
    float m32;
    float m33;
} Mat4;


Vec2 newVec2(float x, float y);
Vec3 newVec3(float x, float y, float z);
Vec3 vec3_add(Vec3 *a, Vec3 *b);
float vec3_dot(Vec3 *a, Vec3 *b);
float vec3_distance(Vec3 *a, Vec3 *b);
void vec3_normalize(Vec3 *vec);
Vec3 vec3_cross(Vec3 v0, Vec3 v1);
int vec3_is_equal(Vec3 a, Vec3 b);
int vec4_is_equal(Vec4 a, Vec4 b);


float lerp(float a, float b, float t);
Vec2 vec2_lerp(Vec2 a, Vec2 b, float t);
Vec3 vec3_lerp(Vec3 a, Vec3 b, float t);

Vec4 newVec4(float x, float y, float z, float w);
Vec4 vec4_lerp(Vec4 a, Vec4 b, float t);
Vec4 vec4_normalize(Vec4 vec);

Quat quat_new(float x, float y, float z, float w);
Quat quat_rotation(Vec3 dir, float angle);
Quat quat_add(Quat a, Quat b);
Quat quat_mult(Quat a, Quat b);
float quat_dot(Quat a, Quat b);
Quat quat_normalize(Quat a);
Quat quat_conjugate(Quat a);
Quat quat_inv(Quat a);
Quat quat_from_euler(float pitch, float yaw, float roll);
Vec3 vec3_rotate_quat(Vec3 v, Quat rot);
Mat4 quat_to_mat4(Quat q);

Mat4 mat4_diag(float x, float y, float z, float w);
Mat4 Mat4I();

Mat4 mat4_copy(Mat4 *A);
Mat4 mat4_add(Mat4 *A, Mat4 *B);
Mat4 mat4_multiply(Mat4 *A, Mat4 *B);
void mat4_scale(Vec3 *vec, Mat4 *source);
Mat4 mat4_translate(Vec3 *vec, Mat4 *A);
Mat4 mat4_transpose(Mat4 *A);
Mat4 mat4_inverse(Mat4 *in);
Vec4 vec4_multiply(Mat4 *A, Vec4 *vec);

Mat4 mat4_rotate_x(float angle, Mat4 *A);
Mat4 mat4_rotate_y(float angle, Mat4 *A);
Mat4 mat4_rotate_z(float angle, Mat4 *A);

Mat4 create_view_matrix(Vec3 *position, float pitch, float yaw);
Mat4 create_transformation_matrix(
    Vec3 *translation,
    float rx, float ry, float rz,
    Vec3 *scale
);

Mat4 create_transformation_matrix_2d(
    float x, float y, float scale_x, float scale_y
);

void print_mat4(char *msg, Mat4 *matrix);
Mat4 mat4_look_at(Vec3 eye, Vec3 centre, Vec3 up);

void log_if_err(char *err_msg);

#endif  // COMMON_H
