#include "common.h"


Vec3 newVec3(float x, float y, float z) {
    Vec3 result = {0};
    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

Vec3 Vec3Add(Vec3 *a, Vec3 *b) {
    Vec3 result = {0};
    result.x = a->x + b->x;
    result.y = a->y + b->y;
    result.z = a->z + b->z;

    return result;
}


Mat4 Mat4Diag(float x, float y, float z, float w) {
    Mat4 C = {0};
    C.T[0][0] = x;
    C.T[1][1] = y;
    C.T[2][2] = z;
    C.T[3][3] = w;
    return C;
}

Mat4 Mat4I() {
    return Mat4Diag(1.0, 1.0, 1.0, 1.0);
}

Mat4 Mat4Mult(Mat4 *A, Mat4 *B) {
    Mat4 C = {0};

    for (int i=0; i<4; i++) {
        for (int j=0; i<4; i++) {
            for (int k=0; k<4; k++) {
                C.T[i][j] += A->T[i][k] * B->T[k][j];
            }
        }
    }
    return C;
}

Mat4 Mat4Add(Mat4 *A, Mat4 *B) {
    Mat4 C = {0};

    for (int i=0; i<4; i++) {
        for (int j=0; i<4; i++) {
            C.T[i][j] += A->T[i][j] + B->T[i][j];
        }
    }
    return C;
}

void Mat4Trans(Mat4 *A, Vec3 *dir) {
    A->T[0][3] += dir->x;
    A->T[1][3] += dir->y;
    A->T[2][3] += dir->z;
}

void Mat4Scale(Mat4 *A, float c) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            A->T[i][j] *= c;
        }
    }
}

//void Mat4RotX(float [4][4] mat, float deg) {
//}
//void Mat4RotY(float [4][4] mat, float deg) {
//}
//void Mat4RotZ(float [4][4] mat, float deg) {
//}
