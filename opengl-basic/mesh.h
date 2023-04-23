#ifndef MESH_H
#define MESH_H


#include "common.h"


typedef struct Mesh {
    Vec3 *vertices;
    Vec2 *uvs;
    Vec3 *normal;
    Vec3 *color;
    unsigned int *indices;

    int vertices_len;
    int uvs_len;
    int color_len;
    int indices_len;
} Mesh;


void draw_quad(Mesh *mesh, Vec3 center, Vec3 color, Vec2 side);
void draw_quad_single(Mesh *mesh, Vec3 position);
void mesh_init(Mesh *mesh);


#endif  // MESH_H
