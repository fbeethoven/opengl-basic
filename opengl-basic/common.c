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


Quat quat_new(float x, float y, float z, float w) {
    Quat result = {.x = x, .y = y, .z = z, .w = w};
    return result;
}


Quat quat_rotation(Vec3 dir, float angle) {
    Quat result = {0};
    Vec3 normalize_dir = dir;
    vec3_normalize(&normalize_dir);
    float s = sinf(angle * 0.5);
    float c = cosf(angle * 0.5);
    result.x = s * normalize_dir.x;
    result.y = s * normalize_dir.y;
    result.z = s * normalize_dir.z;
    result.w = c;

    return result;
}

Quat quat_add(Quat a, Quat b) {
    Quat result = {0};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
    return result;
}

Quat quat_mult(Quat a, Quat b) {
    Quat result = {0};
    result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	result.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	result.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

    return result;
}

Quat quat_from_euler(float pitch, float yaw, float roll) {
    Quat rot_x = quat_rotation(newVec3(1.0, 0.0, 0.0), pitch);
    Quat rot_y = quat_rotation(newVec3(1.0, 0.0, 0.0), yaw);
    Quat rot_z = quat_rotation(newVec3(1.0, 0.0, 0.0), roll);

    Quat result = quat_mult(rot_x, rot_y);
    return quat_mult(result, rot_z);
}

float quat_dot(Quat a, Quat b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

Quat quat_normalize(Quat a) {
    float norm = quat_dot(a, a);
    Quat result = {
        .x = a.x / norm, .y = a.y / norm, .z = a.z / norm, .w = a.w / norm
    };
    return result; 
}

Quat quat_conjugate(Quat a) {
    Quat result = {.x = -a.x, .y = -a.y, .z = -a.z, .w = a.w};
    return result;
}

Quat quat_inv(Quat a) {
    float norm = quat_dot(a, a);
    Quat result = {
        .x = -a.x / norm, .y = -a.y / norm, .z = -a.z / norm, .w = a.w / norm
    };
    return result; 
}

Vec3 vec3_rotate_quat(Vec3 v, Quat rot) {
    Quat tmp = {.x = v.x, .y = v.y, .z = v.z, .w = 0.0};
    tmp = quat_mult(tmp, quat_conjugate(rot));
    tmp = quat_mult(rot, tmp);

    return newVec3(tmp.x, tmp.y, tmp.z);
}

Mat4 quat_to_mat4(Quat q) {
	Mat4 result = {0};
    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    result.m00 = w*w + x*x - y*y - z*z;
    result.m01 = 2*x*y - 2*w*z;
    result.m02 = 2*x*z + 2*w*y;
    result.m03 = 0.0;

    result.m10 = 2*x*y + 2*w*z;
    result.m11 = w*w + y*y - x*x - z*z;
    result.m12 = 2*y*z - 2*w*x;
    result.m13 = 0.0;

    result.m20 = 2*x*z - 2*w*y;
    result.m21 = 2*y*z + 2*w*x;
    result.m22 = w*w + z*z - x*x - y*y;
    result.m23 = 0.0;

    result.m30 = 0.0;
    result.m31 = 0.0;
    result.m32 = 0.0;
    result.m33 = 1.0;

    return result; 
}



int vec3_is_equal(Vec3 a, Vec3 b) {
    return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z)); 
}


int vec4_is_equal(Vec4 a, Vec4 b) {
    return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w)); 
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

    C.m00 = A->m00*B->m00 + A->m01*B->m10 + A->m02*B->m20 + A->m03*B->m30;
    C.m01 = A->m00*B->m01 + A->m01*B->m11 + A->m02*B->m21 + A->m03*B->m31;
    C.m02 = A->m00*B->m02 + A->m01*B->m12 + A->m02*B->m22 + A->m03*B->m32;
    C.m03 = A->m00*B->m03 + A->m01*B->m13 + A->m02*B->m23 + A->m03*B->m33;

    C.m10 = A->m10*B->m01 + A->m11*B->m10 + A->m12*B->m21 + A->m13*B->m31;
    C.m11 = A->m10*B->m01 + A->m11*B->m11 + A->m12*B->m21 + A->m13*B->m31;
    C.m12 = A->m10*B->m02 + A->m11*B->m12 + A->m12*B->m22 + A->m13*B->m32;
    C.m13 = A->m10*B->m03 + A->m11*B->m13 + A->m12*B->m23 + A->m13*B->m33;

    C.m20 = A->m20*B->m00 + A->m21*B->m10 + A->m22*B->m20 + A->m23*B->m30;
    C.m21 = A->m20*B->m01 + A->m21*B->m11 + A->m22*B->m21 + A->m23*B->m31;
    C.m22 = A->m20*B->m02 + A->m21*B->m12 + A->m22*B->m22 + A->m23*B->m32;
    C.m23 = A->m20*B->m03 + A->m21*B->m13 + A->m22*B->m23 + A->m23*B->m33;

    C.m30 = A->m30*B->m00 + A->m31*B->m10 + A->m32*B->m20 + A->m33*B->m30;
    C.m31 = A->m30*B->m01 + A->m31*B->m11 + A->m32*B->m21 + A->m33*B->m31;
    C.m32 = A->m30*B->m02 + A->m31*B->m12 + A->m32*B->m22 + A->m33*B->m32;
    C.m33 = A->m30*B->m03 + A->m31*B->m13 + A->m32*B->m23 + A->m33*B->m33;
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
    float rx, float ry, float rz,
    Vec3 *scale
) {
    Mat4 C = Mat4I();

    C = mat4_translate(translation, &C);

    C = mat4_rotate_x(rx, &C);
    C = mat4_rotate_y(ry, &C);
    C = mat4_rotate_z(rz, &C);

    mat4_scale(scale, &C);
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

Vec4 vec4_normalize(Vec4 vec) {
    float norm = vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w;
    norm = sqrt(norm);
    return newVec4(vec.x / norm, vec.y / norm, vec.z / norm, vec.w / norm);
}

Vec4 vec4_lerp(Vec4 a, Vec4 b, float t) {
    return newVec4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t)
    );
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

Mat4 mat4_transpose(Mat4 *in) {
    Mat4 C = {0};

    C.m00 = in->m00;
    C.m01 = in->m10;
    C.m02 = in->m20;
    C.m03 = in->m30;

    C.m10 = in->m01;
    C.m11 = in->m11;
    C.m12 = in->m21;
    C.m13 = in->m31;

    C.m20 = in->m02;
    C.m21 = in->m12;
    C.m22 = in->m22;
    C.m23 = in->m32;

    C.m30 = in->m03;
    C.m31 = in->m13;
    C.m32 = in->m23;
    C.m33 = in->m33;

    return C;
}

Mat4 mat4_inverse(Mat4 *in) {
    float ood;
    Mat4 o = {0};

	float sf00 = in->m22 * in->m33 - in->m32 * in->m23;
	float sf01 = in->m21 * in->m33 - in->m31 * in->m23;
	float sf02 = in->m21 * in->m32 - in->m31 * in->m22;
	float sf03 = in->m20 * in->m33 - in->m30 * in->m23;
	float sf04 = in->m20 * in->m32 - in->m30 * in->m22;
	float sf05 = in->m20 * in->m31 - in->m30 * in->m21;
	float sf06 = in->m12 * in->m33 - in->m32 * in->m13;
	float sf07 = in->m11 * in->m33 - in->m31 * in->m13;
	float sf08 = in->m11 * in->m32 - in->m31 * in->m12;
	float sf09 = in->m10 * in->m33 - in->m30 * in->m13;
	float sf10 = in->m10 * in->m32 - in->m30 * in->m12;
	float sf11 = in->m11 * in->m33 - in->m31 * in->m13;
	float sf12 = in->m10 * in->m31 - in->m30 * in->m11;
	float sf13 = in->m12 * in->m23 - in->m22 * in->m13;
	float sf14 = in->m11 * in->m23 - in->m21 * in->m13;
	float sf15 = in->m11 * in->m22 - in->m21 * in->m12;
	float sf16 = in->m10 * in->m23 - in->m20 * in->m13;
	float sf17 = in->m10 * in->m22 - in->m20 * in->m12;
	float sf18 = in->m10 * in->m21 - in->m20 * in->m11;

	o.m00 = +(in->m11 * sf00 - in->m12 * sf01 + in->m13 * sf02);
	o.m10 = -(in->m10 * sf00 - in->m12 * sf03 + in->m13 * sf04);
	o.m20 = +(in->m10 * sf01 - in->m11 * sf03 + in->m13 * sf05);
	o.m30 = -(in->m10 * sf02 - in->m11 * sf04 + in->m12 * sf05);

	o.m01 = -(in->m01 * sf00 - in->m02 * sf01 + in->m03 * sf02);
	o.m11 = +(in->m00 * sf00 - in->m02 * sf03 + in->m03 * sf04);
	o.m21 = -(in->m00 * sf01 - in->m01 * sf03 + in->m03 * sf05);
	o.m31 = +(in->m00 * sf02 - in->m01 * sf04 + in->m02 * sf05);

	o.m02 = +(in->m01 * sf06 - in->m02 * sf07 + in->m03 * sf08);
	o.m12 = -(in->m00 * sf06 - in->m02 * sf09 + in->m03 * sf10);
	o.m22 = +(in->m00 * sf11 - in->m01 * sf09 + in->m03 * sf12);
	o.m32 = -(in->m00 * sf08 - in->m01 * sf10 + in->m02 * sf12);

	o.m03 = -(in->m01 * sf13 - in->m02 * sf14 + in->m03 * sf15);
	o.m13 = +(in->m00 * sf13 - in->m02 * sf16 + in->m03 * sf17);
	o.m23 = -(in->m00 * sf14 - in->m01 * sf16 + in->m03 * sf18);
	o.m33 = +(in->m00 * sf15 - in->m01 * sf17 + in->m02 * sf18);

	ood = 1.0f / (in->m00 * o.m00 +
				  in->m01 * o.m10 +
				  in->m02 * o.m20 +
				  in->m03 * o.m30);

	o.m00 *= ood;
	o.m01 *= ood;
	o.m02 *= ood;
	o.m03 *= ood;
	o.m10 *= ood;
	o.m11 *= ood;
	o.m12 *= ood;
	o.m13 *= ood;
	o.m20 *= ood;
	o.m21 *= ood;
	o.m22 *= ood;
	o.m23 *= ood;
	o.m30 *= ood;
	o.m31 *= ood;
	o.m32 *= ood;
    o.m33 *= ood;

    return o;
}


void log_if_err(char *err_msg) {
    int err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("[ERROR: %d] GL Error: %s", err, err_msg);
        // exit(1);
    }
}

