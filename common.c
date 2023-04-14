#include "common.h"


Vec2 newVec2(float x, float y) {
    Vec2 result = {0};
    result.x = x;
    result.y = y;

    return result;
}


Vec3 newVec3(float x, float y, float z) {
    Vec3 result = {0};
    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}


Vec4 newVec4(float x, float y, float z, float w) {
    Vec4 result = {0};
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;

    return result;
}


Vec3 vec3_add(Vec3 *a, Vec3 *b) {
    Vec3 result = {0};
    result.x = a->x + b->x;
    result.y = a->y + b->y;
    result.z = a->z + b->z;

    return result;
}


Mat4 mat4_diag(float x, float y, float z, float w) {
    Mat4 C = {0};
    C.m00 = x;
    C.m11 = y;
    C.m22 = z;
    C.m33 = w;
    return C;
}

Mat4 Mat4I() {
    return mat4_diag(1.0, 1.0, 1.0, 1.0);
}


Vec4 vec4_multiply(Mat4 *A, Vec4 *vec) {
    Vec4 C = {0};

    C.x = A->m00*vec->x + A->m01*vec->y + A->m02*vec->z + A->m03*vec->w;
    C.y = A->m10*vec->x + A->m11*vec->y + A->m12*vec->z + A->m13*vec->w;
    C.z = A->m20*vec->x + A->m21*vec->y + A->m22*vec->z + A->m23*vec->w;
    C.w = A->m30*vec->x + A->m31*vec->y + A->m32*vec->z + A->m33*vec->w;
    return C;
}

Mat4 mat4_multiply(Mat4 *A, Mat4 *B) {
    Mat4 C = {0};

    C.m00 = A->m00*B->m00 + A->m01*B->m10 + A->m02*B->m20+ A->m03*B->m30;
    C.m01 = A->m00*B->m01 + A->m01*B->m11 + A->m02*B->m21+ A->m03*B->m31;
    C.m02 = A->m00*B->m02 + A->m01*B->m12 + A->m02*B->m22+ A->m03*B->m32;
    C.m03 = A->m00*B->m03 + A->m01*B->m13 + A->m02*B->m23+ A->m03*B->m33;

    C.m10 = A->m10*B->m01 + A->m11*B->m10 + A->m12*B->m21+ A->m13*B->m31;
    C.m11 = A->m10*B->m01 + A->m11*B->m11 + A->m12*B->m21+ A->m13*B->m31;
    C.m12 = A->m10*B->m02 + A->m11*B->m12 + A->m12*B->m22+ A->m13*B->m32;
    C.m13 = A->m10*B->m03 + A->m11*B->m13 + A->m12*B->m23+ A->m13*B->m33;

    C.m20 = A->m20*B->m00 + A->m21*B->m10 + A->m22*B->m20+ A->m23*B->m30;
    C.m21 = A->m20*B->m01 + A->m21*B->m11 + A->m22*B->m21+ A->m23*B->m31;
    C.m22 = A->m20*B->m02 + A->m21*B->m12 + A->m22*B->m22+ A->m23*B->m32;
    C.m23 = A->m20*B->m03 + A->m21*B->m13 + A->m22*B->m23+ A->m23*B->m33;

    C.m30 = A->m30*B->m00 + A->m31*B->m10 + A->m32*B->m20+ A->m33*B->m30;
    C.m31 = A->m30*B->m01 + A->m31*B->m11 + A->m32*B->m21+ A->m33*B->m31;
    C.m32 = A->m30*B->m02 + A->m31*B->m12 + A->m32*B->m22+ A->m33*B->m32;
    C.m33 = A->m30*B->m03 + A->m31*B->m13 + A->m32*B->m23+ A->m33*B->m33;
    return C;
}


void mat4_scale(Vec3 *vec, Mat4 *source) {
    source->m00 = vec->x * source->m00;
    source->m01 = vec->x * source->m01;
    source->m02 = vec->x * source->m02;
    source->m03 = vec->x * source->m03;

    source->m10 = vec->y * source->m10;
    source->m11 = vec->y * source->m11;
    source->m12 = vec->y * source->m12;
    source->m13 = vec->y * source->m13;

    source->m20 = vec->z * source->m20;
    source->m21 = vec->z * source->m21;
    source->m22 = vec->z * source->m22;
    source->m23 = vec->z * source->m23;
}

Mat4 mat4_copy(Mat4 *A) {
    Mat4 C = {0};
    C.m00 = A->m00;
    C.m01 = A->m01;
    C.m02 = A->m02;
    C.m03 = A->m03;

    C.m10 = A->m10;
    C.m11 = A->m11;
    C.m12 = A->m12;
    C.m13 = A->m13;

    C.m20 = A->m20;
    C.m21 = A->m21;
    C.m22 = A->m22;
    C.m23 = A->m23;

    C.m30 = A->m30;
    C.m31 = A->m31;
    C.m32 = A->m32;
    C.m33 = A->m33;

    return C;
}

Mat4 mat4_rotate_x(float angle, Mat4 *A) {
    Mat4 C = Mat4I();

    float c = cos(angle);
    float s = sin(angle);

    float f00 = 1;
    float f01 = 0;
    float f02 = 0;
    float f10 = 0;
    float f11 = c;
    float f12 = s;
    float f20 = 0;
    float f21 = -s;
    float f22 = c;

    float t00 = A->m00 * f00 + A->m10 * f01 + A->m20 * f02;
    float t01 = A->m01 * f00 + A->m11 * f01 + A->m21 * f02;
    float t02 = A->m02 * f00 + A->m12 * f01 + A->m22 * f02;
    float t03 = A->m03 * f00 + A->m13 * f01 + A->m23 * f02;

    float t10 = A->m00 * f10 + A->m10 * f11 + A->m20 * f12;
    float t11 = A->m01 * f10 + A->m11 * f11 + A->m21 * f12;
    float t12 = A->m02 * f10 + A->m12 * f11 + A->m22 * f12;
    float t13 = A->m03 * f10 + A->m13 * f11 + A->m23 * f12;

    C.m20 = A->m00 * f20 + A->m10 * f21 + A->m20 * f22;
    C.m21 = A->m01 * f20 + A->m11 * f21 + A->m21 * f22;
    C.m22 = A->m02 * f20 + A->m12 * f21 + A->m22 * f22;
    C.m23 = A->m03 * f20 + A->m13 * f21 + A->m23 * f22;
    C.m00 = t00;
    C.m01 = t01;
    C.m02 = t02;
    C.m03 = t03;
    C.m10 = t10;
    C.m11 = t11;
    C.m12 = t12;
    C.m13 = t13;

    C.m30 = A->m30;
    C.m31 = A->m31;
    C.m32 = A->m32;
    C.m33 = A->m33;

    return C;
}

Mat4 mat4_rotate_y(float angle, Mat4 *A) {
    Mat4 C = Mat4I();

    float c = cos(angle);
    float s = sin(angle);

    float f00 = c;
    float f01 = 0;
    float f02 = -s;
    float f10 = 0;
    float f11 = 1;
    float f12 = 0;
    float f20 = s;
    float f21 = 0;
    float f22 = c;

    float t00 = A->m00 * f00 + A->m10 * f01 + A->m20 * f02;
    float t01 = A->m01 * f00 + A->m11 * f01 + A->m21 * f02;
    float t02 = A->m02 * f00 + A->m12 * f01 + A->m22 * f02;
    float t03 = A->m03 * f00 + A->m13 * f01 + A->m23 * f02;

    float t10 = A->m00 * f10 + A->m10 * f11 + A->m20 * f12;
    float t11 = A->m01 * f10 + A->m11 * f11 + A->m21 * f12;
    float t12 = A->m02 * f10 + A->m12 * f11 + A->m22 * f12;
    float t13 = A->m03 * f10 + A->m13 * f11 + A->m23 * f12;

    C.m20 = A->m00 * f20 + A->m10 * f21 + A->m20 * f22;
    C.m21 = A->m01 * f20 + A->m11 * f21 + A->m21 * f22;
    C.m22 = A->m02 * f20 + A->m12 * f21 + A->m22 * f22;
    C.m23 = A->m03 * f20 + A->m13 * f21 + A->m23 * f22;
    C.m00 = t00;
    C.m01 = t01;
    C.m02 = t02;
    C.m03 = t03;
    C.m10 = t10;
    C.m11 = t11;
    C.m12 = t12;
    C.m13 = t13;

    C.m30 = A->m30;
    C.m31 = A->m31;
    C.m32 = A->m32;
    C.m33 = A->m33;

    return C;
}

Mat4 mat4_rotate_z(float angle, Mat4 *A) {
    Mat4 C = Mat4I();

    float c = cos(angle);
    float s = sin(angle);

    float f00 = c;
    float f01 = s;
    float f02 = 0;
    float f10 = -s;
    float f11 = c;
    float f12 = 0;
    float f20 = 0;
    float f21 = 0;
    float f22 = 1;

    float t00 = A->m00 * f00 + A->m10 * f01 + A->m20 * f02;
    float t01 = A->m01 * f00 + A->m11 * f01 + A->m21 * f02;
    float t02 = A->m02 * f00 + A->m12 * f01 + A->m22 * f02;
    float t03 = A->m03 * f00 + A->m13 * f01 + A->m23 * f02;

    float t10 = A->m00 * f10 + A->m10 * f11 + A->m20 * f12;
    float t11 = A->m01 * f10 + A->m11 * f11 + A->m21 * f12;
    float t12 = A->m02 * f10 + A->m12 * f11 + A->m22 * f12;
    float t13 = A->m03 * f10 + A->m13 * f11 + A->m23 * f12;

    C.m20 = A->m00 * f20 + A->m10 * f21 + A->m20 * f22;
    C.m21 = A->m01 * f20 + A->m11 * f21 + A->m21 * f22;
    C.m22 = A->m02 * f20 + A->m12 * f21 + A->m22 * f22;
    C.m23 = A->m03 * f20 + A->m13 * f21 + A->m23 * f22;
    C.m00 = t00;
    C.m01 = t01;
    C.m02 = t02;
    C.m03 = t03;
    C.m10 = t10;
    C.m11 = t11;
    C.m12 = t12;
    C.m13 = t13;

    C.m30 = A->m30;
    C.m31 = A->m31;
    C.m32 = A->m32;
    C.m33 = A->m33;

    return C;
}


Mat4 mat4_translate(Vec3 *vec, Mat4 *A) {
    Mat4 C = Mat4I();
    C.m30 += A->m00 * vec->x + A->m10 * vec->y + A->m20 * vec->z;
    C.m31 += A->m01 * vec->x + A->m11 * vec->y + A->m21 * vec->z;
    C.m32 += A->m02 * vec->x + A->m12 * vec->y + A->m22 * vec->z;
    C.m33 += A->m03 * vec->x + A->m13 * vec->y + A->m23 * vec->z;
    return C;
}


Mat4 create_transformation_matrix(
    Vec3 *translation,
    float rx, float ry, float rz, float scale_factor
) {
    Mat4 C = Mat4I();

    C = mat4_translate(translation, &C);

    C = mat4_rotate_x(rx, &C);
    C = mat4_rotate_y(ry, &C);
    C = mat4_rotate_z(rz, &C);

    Vec3 scale_vec = newVec3(scale_factor, scale_factor, scale_factor);
    mat4_scale(&scale_vec, &C);
    return C;
}

Mat4 create_transformation_matrix_2d(
    float x, float y, float scale_x, float scale_y
) {
    Mat4 C = Mat4I();

    Vec3 translate_vec = newVec3(x, y, 0);
    C = mat4_translate(&translate_vec, &C);


    Vec3 scale_vec = newVec3(scale_x, scale_y, 1.0);
	mat4_scale(&scale_vec, &C);
	return C;
}

Mat4 create_view_matrix(Vec3 *pos, float pitch, float yaw) {
    Mat4 C = Mat4I();

    Vec3 neg_camera_position = newVec3(-1 * pos->x, -1 * pos->y, -1 * pos->z);
    C = mat4_translate(&neg_camera_position, &C);

    C = mat4_rotate_x(pitch, &C);
    C = mat4_rotate_y(yaw, &C);
    C = mat4_rotate_z(0.0, &C);


    return C;
}

void print_mat4(char *msg, Mat4 *matrix) {
    printf("%s\n", msg);

    printf("%f %f %f %f\n", matrix->m00, matrix->m01, matrix->m02, matrix->m03);
    printf("%f %f %f %f\n", matrix->m10, matrix->m11, matrix->m12, matrix->m13);
    printf("%f %f %f %f\n", matrix->m20, matrix->m21, matrix->m22, matrix->m23);
    printf("%f %f %f %f\n", matrix->m30, matrix->m31, matrix->m32, matrix->m33);
}

Vec3 vec3_cross(Vec3 v0, Vec3 v1) {
    Vec3 result = {0};
    result.x = v0.y*v1.z - v0.z*v1.y;
	result.y = v0.z*v1.x - v0.x*v1.z;
	result.z = v0.x*v1.y - v0.y*v1.x;
    return result;
}


float lerp(float a, float b, float t) {
    return a + t*(b-a);
}

Vec2 vec2_lerp(Vec2 a, Vec2 b, float t) {
    return newVec2(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
}

Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
    return newVec3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
}


float vec3_dot(Vec3 *a, Vec3 *b) {
    float norm = a->x *b->x + a->y*b->y + a->z*b->z;
    return norm;
}


float vec3_distance(Vec3 *a, Vec3 *b) {
    Vec3 a_to_b = newVec3(b->x - a->x, b->y - a->y, b->z - a->z);
    return sqrt(vec3_dot(&a_to_b, &a_to_b));
}


void vec3_normalize(Vec3 *vec) {
    float norm = vec->x*vec->x + vec->y*vec->y + vec->z*vec->z;
    norm = sqrt(norm);
    vec->x /= norm;
    vec->y /= norm;
    vec->z /= norm;
}


Mat4 mat4_look_at(Vec3 eye, Vec3 centre, Vec3 up) {
    Vec3 f = newVec3(centre.x - eye.x, centre.y - eye.y, centre.z - eye.z);
    vec3_normalize(&f);

	Vec3 s = vec3_cross(f, up);
    vec3_normalize(&s);

	Vec3 u = vec3_cross(s, f);

    Mat4 C = Mat4I();

	C.m00 = +s.x;
	C.m10 = +s.y;
	C.m20 = +s.z;

	C.m01 = +u.x;
	C.m11 = +u.y;
	C.m21 = +u.z;

	C.m02 = -f.x;
	C.m12 = -f.y;
	C.m22 = -f.z;

	C.m30 = -(s.x*eye.x + s.y*eye.y + s.z*eye.z);
	C.m31 = -(u.x*eye.x + u.y*eye.y + u.z*eye.z);
	C.m32 = +(f.x*eye.x + f.y*eye.y + f.z*eye.z);

    return C;
}


void log_if_err(char *err_msg) {
    int err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("[ERROR: %d] GL Error: %s", err, err_msg);
        exit(1);
    }
}

