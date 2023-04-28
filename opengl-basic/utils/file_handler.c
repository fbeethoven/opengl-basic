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


typedef struct Tokenizer {
    char *data;
    int cursor;
    int lines;
} Tokenizer;


void skip_white_space(Tokenizer *tokenizer) {
    char c = tokenizer->data[tokenizer->cursor];
    while(c && ( c == ' ' || c == '\n' || c == '\r') ) {
        if (c == '\n') {
            tokenizer->lines++;
        }
        c = tokenizer->data[++tokenizer->cursor];
    }
}

Token parse_str_literal(Tokenizer *tokenizer) {
    Token result = {0};
    result.kind = Token_StrLiteral;

    int i = 0;
    char c = tokenizer->data[tokenizer->cursor++];
    while(c && c != '"') {
        result.info[i++] = c;
        c = tokenizer->data[tokenizer->cursor++];

        if (i == 255) {
            printf("Token info out of memory parsing %s\n", result.info);
            exit(1);
        }
    }
    return result;
}

int char_is_numeric(char c) {
    return (c == '-') || (c == 'e') || (('0' <= c) && (c <= '9'));
}

Token parse_num(Tokenizer *tokenizer) {
    Token result = {0};
    result.kind = Token_Int;
    int i = 0;

    char c = tokenizer->data[tokenizer->cursor];
    while(c && ( char_is_numeric(c)|| c == '.') ) {
        if (c == '.') {
            result.kind = Token_Float;
        }
        result.info[i++] = c;
        c = tokenizer->data[++tokenizer->cursor];

        if (i == 255) {
            printf("Token info out of memory parsing %s\n", result.info);
            exit(1);
        }
    }
    return result;
}

Token parse_true(Tokenizer *tokenizer) {
    Token result = {0};
    result.kind = Token_Bool;
    for (int i=0; i<4; i++) {
        result.info[i] = tokenizer->data[tokenizer->cursor++];
    }
    if (strcmp(result.info, "true") != 0) {
        printf("Expected true, got %s\n", result.info);
        exit(0);
    }
    return result;
}

Token parse_false(Tokenizer *tokenizer) {
    Token result = {0};
    result.kind = Token_Bool;
    for (int i=0; i<5; i++) {
        result.info[i] = tokenizer->data[tokenizer->cursor++];
    }
    if (strcmp(result.info, "false") != 0) {
        printf("Expected false, got %s\n", result.info);
        exit(0);
    }
    return result;
}

Token token_next(Tokenizer *tokenizer) {
    Token new_token = {0};
    skip_white_space(tokenizer);

    char c = tokenizer->data[tokenizer->cursor];

    if (char_is_numeric(c)) {
        new_token = parse_num(tokenizer);
    }
    else {
        switch(c) {
            case '{':
                tokenizer->cursor++;
                new_token.kind = Token_OpenCurl;
                break;
            case '}':
                tokenizer->cursor++;
                new_token.kind = Token_CloseCurl;
                break;
            case '[':
                tokenizer->cursor++;
                new_token.kind = Token_OpenBra;
                break;
            case ']':
                tokenizer->cursor++;
                new_token.kind = Token_CloseBra;
                break;
            case ':':
                tokenizer->cursor++;
                new_token.kind = Token_Colon;
                break;
            case ',':
                tokenizer->cursor++;
                new_token.kind = Token_Coma;
                break;
            case '"':
                tokenizer->cursor++;
                new_token = parse_str_literal(tokenizer);
                break;
            case 't':
                new_token = parse_true(tokenizer);
                break;
            case 'f':
                new_token = parse_false(tokenizer);
                break;
            case '\0':
                new_token.kind = Token_EOF;
                break;
            default:
                printf(
                    "Unknown Token %c in line %d, position %d\n",
                    c, tokenizer->lines, tokenizer->cursor
                );
                exit(1);
        }
    }

    return new_token; 
}

ArrayList *tokenize_input(char *data) {
    Tokenizer tokenizer = {0};
    tokenizer.data = data;

    ArrayList *tokens = new_array_list(Token); 
    for(
        Token *token = arr_push(tokens, Token);
        tokenizer.data[tokenizer.cursor];
        *token = token_next(&tokenizer)
    ) {
        token = arr_push(tokens, Token);
    }
    
    return tokens;
}

typedef struct HashNode HashNode;
struct HashNode {
    char name[50];
    Token first;
    HashNode *info_next;
    HashNode *hash_next;
};


int main() {
    char *file_path = "assets/models/scene.gltf";
    char *data = read_file(file_path);

    ArrayList *tokens = tokenize_input(data);



    char *TypeNames[] = {
        "Token_Unknown",
        "Token_StrLiteral",
        "Token_Colon",
        "Token_Coma",
        "Token_Int",
        "Token_Float",
        "Token_Bool",
        "Token_OpenCurl",
        "Token_CloseCurl",
        "Token_OpenBra",
        "Token_CloseBra",
        "Token_EOF"
    };

    Token token;
    for (int i=0; i<tokens->counter; i++) {
        token = ((Token *)tokens->data)[i];
        printf("TOKEN(%s, %s)\n", TypeNames[token.kind], token.info);
    }

    return 0;
}
