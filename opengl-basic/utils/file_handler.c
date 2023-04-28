#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_handler.h"
#include "../memory.h"


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
        if (pointer == (BufferCapacity - 1)) {
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
        fprintf(stderr, "Failed to read file: %s\n", file_path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    char *data = malloc(sizeof(char) * (file_size + 1));
    if(fread(data, sizeof(char), file_size, file) != file_size) {
        fprintf(
            stderr, "there was an error reading input of file %s\n", file_path
        );
        free(data);
        fclose(file);
        exit(1);
    }

    data[file_size] = 0;
    fclose(file);
    return data;
}


void handle_face_vertex(
    char *face_vertex,
    IntermediateModel *dest,
    ArrayList *normals,
    ArrayList *uvs,
    ArrayList *indices,
    int simple
) {
    unsigned int vertex_pointer;
    if (simple) {
        vertex_pointer = (unsigned int) atoi(face_vertex) - 1;
        *arr_push(indices, unsigned int) = vertex_pointer;
        dest->uvs[2*vertex_pointer] = 0.0;
        dest->uvs[2*vertex_pointer + 1] = 0.0;

        dest->normals[3*vertex_pointer] = 0.0;
        dest->normals[3*vertex_pointer + 1] = 1.0;
        dest->normals[3*vertex_pointer + 2] = 0.0;
    }
    else {
        StrSplitter split_bar = {0};
        split_bar.separator = '/';

        split_next(&split_bar, face_vertex);
        vertex_pointer = (unsigned int) atoi(split_bar.buffer) - 1;

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
}


void _parse_obj_file(char *file_path, IntermediateModel *dest, int simple) {
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
                split_space.buffer, dest, normals, uvs, indices, simple
            );

            split_next(&split_space, split_line.buffer);
            handle_face_vertex(
                split_space.buffer, dest, normals, uvs, indices, simple
            );

            split_next(&split_space, split_line.buffer);
            handle_face_vertex(
                split_space.buffer, dest, normals, uvs, indices, simple
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

void parse_obj_file_simple(char *file_path, IntermediateModel *dest) {
    _parse_obj_file(file_path, dest, 1);
}


void parse_obj_file(char *file_path, IntermediateModel *dest) {
    _parse_obj_file(file_path, dest, 0);
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


void transform_obj_file(char *file_path, char *file_output) {
    char *data = read_file(file_path);
    FILE *out_file = fopen(file_output, "wb");
    char *face_buffer = malloc(500000*sizeof(char));
    int cursor = 0;

    char vertex_1[50];
    char vertex_2[50];
    char vertex_3[50];
    char vertex_4[50];

    StrSplitter split_line = {0};
    split_line.separator = '\n';

    StrSplitter split_space = {0};
    split_space.separator = ' ';

    int lines = 0;

    while (split_next(&split_line, data) >= 0) {
        splitter_reset(&split_space);
        split_next(&split_space, split_line.buffer);
        if (strcmp(split_space.buffer, "f") == 0) {
            split_next(&split_space, split_line.buffer);
            strcpy(vertex_1, split_space.buffer);

            split_next(&split_space, split_line.buffer);
            strcpy(vertex_2, split_space.buffer);

            split_next(&split_space, split_line.buffer);
            strcpy(vertex_3, split_space.buffer);

            split_next(&split_space, split_line.buffer);
            strcpy(vertex_4, split_space.buffer);

            if (*vertex_4 == '\0') {
                cursor += sprintf(
                    face_buffer + cursor, "%s\n", split_line.buffer
                );
            } 
            else {
                cursor += sprintf(
                    face_buffer + cursor,
                    "f %s %s %s\n",
                    vertex_1, vertex_2, vertex_3
                );
                cursor += sprintf(
                    face_buffer + cursor,
                    "f %s %s %s\n",
                    vertex_1, vertex_3, vertex_4
                );
            }
        }
        else {
            fprintf(out_file, "%s\n", split_line.buffer);
        }
        lines++;
    }

    fprintf(out_file, "\n%s\n", face_buffer);

    fclose(out_file);
}
