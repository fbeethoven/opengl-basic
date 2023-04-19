#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_handler.h"
#include "../memory.h"


int push_float(
    float *dest,
    float x,
    int limit,
    unsigned int *counter
) {
    if (*counter == VerticesCapacity) {
        return -1;
    }
    dest[(*counter)++] = x;
    return 0;
}

int push_vertice(IntermediateModel *dest, float x) {
    return push_float(
        dest->vertices, x, VerticesCapacity, &dest->vertices_count
    );
}

int push_normal(IntermediateModel *dest, float x) {
    return push_float(
        dest->normals, x, NormalsCapacity, &dest->normals_count
    );
}

int push_text_coords(IntermediateModel *dest, float x) {
    return push_float(
        dest->uvs, x, TexCapacity, &dest->uvs_count
    );
}

int push_vertice_bac(IntermediateModel *dest, float x) {
    if (dest->vertices_count == VerticesCapacity) {
        return -1;
    }
    dest->vertices[dest->vertices_count++] = x;
    return 0;
}


int push_index(IntermediateModel *dest, unsigned int x) {
    if (dest->indices_count == IndicesCapacity) {
        return -1;
    }
    dest->indices[dest->indices_count++] = x;
    return 0;
}


void splitter_reset(StrSplitter *splitter) {
    splitter->cursor = 0;
    memset(splitter->buffer, 0, BufferCapacity * sizeof(char));
}


int split_next(StrSplitter *splitter, char *data) {
    int pointer = 0;
    memset(splitter->buffer, 0, BufferCapacity * sizeof(char));
    while (
        data[splitter->cursor] &&
        data[splitter->cursor] != splitter->separator
     ) {
        splitter->buffer[pointer] = data[splitter->cursor];
        pointer++;
        splitter->cursor++;
        if (pointer == 499) {
            splitter->buffer[pointer] = '\0';
            return -1; 
        }
    }
    splitter->buffer[pointer] = '\0';
    splitter->cursor++;
    if (!data[splitter->cursor]) {
        return -2;
    }
    return 0;
}


char *read_file(char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to read input\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    char *data = malloc(sizeof(char) * (file_size + 1));
    if(fread(data, sizeof(char), file_size, file) != file_size) {
        fprintf(stderr, "there was an error reading input of file %s\n", file_path);
        free(data);
        fclose(file);
        exit(1);
    }

    data[file_size] = 0;
    fclose(file);
    return data;
}


void parse_obj_file_simple(char *file_path, IntermediateModel *dest) {
    char *data = read_file(file_path);
    StrSplitter split_line = {0};
    split_line.separator = '\n';
    StrSplitter split_space = {0};
    split_space.separator = ' ';

    while (split_next(&split_line, data) >= 0) {
        splitter_reset(&split_space);
        if (split_line.buffer[0] == 'v') {
            split_next(&split_space, split_line.buffer);

            split_next(&split_space, split_line.buffer);
            push_vertice(dest, atof(split_space.buffer));

            split_next(&split_space, split_line.buffer);
            push_vertice(dest, atof(split_space.buffer));

            split_next(&split_space, split_line.buffer);
            push_vertice(dest, atof(split_space.buffer));
        }
        else if ( split_line.buffer[0] == 'f') {
            split_next(&split_space, split_line.buffer);

            split_next(&split_space, split_line.buffer);
            push_index(dest, (unsigned int) atoi(split_space.buffer));

            split_next(&split_space, split_line.buffer);
            push_index(dest, (unsigned int) atoi(split_space.buffer));

            split_next(&split_space, split_line.buffer);
            push_index(dest, (unsigned int) atoi(split_space.buffer));
        }
    }
    free(data);
}


void handle_face_vertex(
    char *face_vertex,
    IntermediateModel *dest,
    ArrayList *normals,
    ArrayList *uvs,
    ArrayList *indices
) {
    StrSplitter split_bar = {0};
    split_bar.separator = '/';

    split_next(&split_bar, face_vertex);
    unsigned int vertex_pointer = (unsigned int) atoi(split_bar.buffer) - 1;

    *arr_push(indices, unsigned int) = vertex_pointer;

    split_next(&split_bar, face_vertex);
    unsigned int uv_pointer = (unsigned int) atoi(split_bar.buffer) - 1;
    dest->uvs[2*vertex_pointer] = (
        ((float *)uvs->data)[2*uv_pointer]
    );
    dest->uvs[2*vertex_pointer + 1] = (
        1.0 - ((float *)uvs->data)[2*uv_pointer + 1]
    );

    split_next(&split_bar, face_vertex);
    unsigned int normal_pointer = (unsigned int) atoi(split_bar.buffer) - 1;
    dest->normals[3*vertex_pointer] = (
        ((float *)normals->data)[3*normal_pointer]
    );
    dest->normals[3*vertex_pointer + 1] = ( 
        ((float *)normals->data)[3*normal_pointer + 1]
    );
    dest->normals[3*vertex_pointer + 2] = ( 
        ((float *)normals->data)[3*normal_pointer + 2]
    );
}


void parse_obj_file(char *file_path, IntermediateModel *dest) {
    ArrayList *vertices = new_array_list(float);
    ArrayList *normals = new_array_list(float);
    ArrayList *uvs = new_array_list(float);
    ArrayList *indices = new_array_list(unsigned int);

    char *data = read_file(file_path);
    StrSplitter split_line = {0};
    split_line.separator = '\n';
    StrSplitter split_space = {0};
    split_space.separator = ' ';

    int lines = 0;
    int undefined_arrays = 0;

    while (split_next(&split_line, data) >= 0) {
        splitter_reset(&split_space);
        split_next(&split_space, split_line.buffer);

        if (strcmp(split_space.buffer, "v") == 0) {

            split_next(&split_space, split_line.buffer);
            *arr_push(vertices, float) = atof(split_space.buffer);

            split_next(&split_space, split_line.buffer);
            *arr_push(vertices, float) = atof(split_space.buffer);

            split_next(&split_space, split_line.buffer);
            *arr_push(vertices, float) = atof(split_space.buffer);
        }
        else if (strcmp(split_space.buffer, "vt") == 0) {

            split_next(&split_space, split_line.buffer);
            *arr_push(uvs, float) = atof(split_space.buffer);

            split_next(&split_space, split_line.buffer);
            *arr_push(uvs, float) = atof(split_space.buffer);

        }
        if (strcmp(split_space.buffer, "vn") == 0) {

            split_next(&split_space, split_line.buffer);
            *arr_push(normals, float) = atof(split_space.buffer);

            split_next(&split_space, split_line.buffer);
            *arr_push(normals, float) = atof(split_space.buffer);

            split_next(&split_space, split_line.buffer);
            *arr_push(normals, float) = atof(split_space.buffer);
        }
        else if (strcmp(split_space.buffer, "f") == 0) {

            if (!undefined_arrays) {
                unsigned int total_vertices = vertices->counter / 3;
                dest->uvs = (
                    (float *)malloc(2 * total_vertices * sizeof(float))
                );
                dest->uvs_count = 2 * total_vertices;
                dest->normals = (
                    (float *)malloc(3 * total_vertices * sizeof(float))
                );
                dest->normals_count = 3 * total_vertices;
                dest->vertices = vertices->data;
                dest->vertices_count = 3 * total_vertices;

                undefined_arrays = 1;
            }

            split_next(&split_space, split_line.buffer);
            handle_face_vertex(
                split_space.buffer, dest, normals, uvs, indices
            );

            split_next(&split_space, split_line.buffer);
            handle_face_vertex(
                split_space.buffer, dest, normals, uvs, indices
            );

            split_next(&split_space, split_line.buffer);
            handle_face_vertex(
                split_space.buffer, dest, normals, uvs, indices
            );
        }
        lines++;
    }

    dest->indices = indices->data;
    dest->indices_count = indices->counter;

    free(indices);
    free(vertices);
    arr_free(normals);
    arr_free(uvs);
    free(data);

    printf(
        "Read %d lines, %d vertices, %d indices\n", 
        lines, dest->vertices_count, dest->indices_count
    );
    printf(
        " %d normal vectors, %d texture coordinates\n", 
        dest->normals_count, dest->uvs_count
    );
}


void intermediate_model_init(IntermediateModel *model) {
    model->vertices = (float *)malloc(3*sizeof(float)*VerticesCapacity);
    model->normals = (float *)malloc(3*sizeof(float)*NormalsCapacity);
    model->uvs = (float *)malloc(3*sizeof(float)*TexCapacity);
    model->indices = (unsigned int *)malloc(3*sizeof(float)*IndicesCapacity);
}


void intermediate_model_free(IntermediateModel *model) {
    free(model->vertices);
    free(model->normals);
    free(model->uvs);
    free(model->indices);
}

