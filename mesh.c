#include "mesh.h"


void draw_quad(Mesh *mesh, Vec3 position) {
    float width = 5.0;
    int v_c = mesh->vertices_len;
    int u_c = mesh->uvs_len;
    int i_c = mesh->indices_len;
    mesh->vertices[v_c] = newVec3(position.x - width, 0, position.z - width);
    mesh->vertices[v_c + 1] = newVec3(position.x - width, 0, position.z + width);
    mesh->vertices[v_c + 2] = newVec3(position.x + width, 0, position.z - width);
    mesh->vertices[v_c + 3] = newVec3(position.x + width, 0, position.z + width);

    mesh->uvs[u_c] = newVec2(0.0, 0.0);
    mesh->uvs[u_c + 1] = newVec2(0.0, 1.0);
    mesh->uvs[u_c + 2] = newVec2(1.0, 0.0);
    mesh->uvs[u_c + 3] = newVec2(1.0, 1.0);

    mesh->indices[i_c] = v_c;
    mesh->indices[i_c + 1] = v_c + 1;
    mesh->indices[i_c + 2] = v_c + 2;

    mesh->indices[i_c + 3] = v_c + 1;
    mesh->indices[i_c + 4] = v_c + 3;
    mesh->indices[i_c + 5] = v_c + 2;


    mesh->vertices_len += 4;
    mesh->uvs_len += 4;
    mesh->indices_len += 6;
}


void mesh_init(Mesh *mesh) {
    float tile_size = 10.0;
    int tiles_per_chunk = 10;
    for (int z=-tiles_per_chunk; z<tiles_per_chunk; z++) {
        for (int x=-tiles_per_chunk; x<tiles_per_chunk; x++) {
            draw_quad(mesh, newVec3(x * tile_size, 0, z * tile_size));
        }
    }
}


