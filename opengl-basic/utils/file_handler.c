#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_handler.h"


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


void tokenizer_reset(Tokenizer *tokenizer) {
    tokenizer->cursor = 0;
    tokenizer->lines = 0;
}


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
    return (c == '-') || (('0' <= c) && (c <= '9'));
}

Token parse_num(Tokenizer *tokenizer) {
    Token result = {0};
    result.kind = Token_Int;
    int i = 0;

    char c = tokenizer->data[tokenizer->cursor];
    while(c && ( char_is_numeric(c)|| c == 'e' || c == '.') ) {
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

int char_is_alpha(char c) {
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}


Token parse_declaration(Tokenizer *tokenizer) {
    Token result = {0};
    result.kind = Token_Declaration;

    int i = 0;
    char c = tokenizer->data[tokenizer->cursor++];
    while(c && c != ' ' && c != '\0') {
        result.info[i++] = c;
        c = tokenizer->data[tokenizer->cursor++];

        if (i == 255) {
            printf("Token info out of memory parsing %s\n", result.info);
            exit(1);
        }
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
    else if (char_is_alpha(c)) {
        new_token = parse_declaration(tokenizer);
        if (strcmp(new_token.info, "true") == 0) {
            new_token.kind = Token_Bool;
        }
    }
    else {
        switch(c) {
            case '{':
                tokenizer->cursor++;
                new_token.kind = Token_OpenCurl;
                new_token.info[0] = '{';
                break;
            case '}':
                tokenizer->cursor++;
                new_token.kind = Token_CloseCurl;
                new_token.info[0] = '}';
                break;
            case '[':
                tokenizer->cursor++;
                new_token.kind = Token_OpenBra;
                new_token.info[0] = '[';
                break;
            case ']':
                tokenizer->cursor++;
                new_token.kind = Token_CloseBra;
                new_token.info[0] = ']';
                break;
            case ':':
                tokenizer->cursor++;
                new_token.kind = Token_Colon;
                new_token.info[0] = ':';
                break;
            case ',':
                tokenizer->cursor++;
                new_token.kind = Token_Coma;
                new_token.info[0] = ',';
                break;
            case '"':
                tokenizer->cursor++;
                new_token = parse_str_literal(tokenizer);
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


char *TypeNames[] = {
    "Token_Unknown",
    "Token_StrLiteral",
    "Token_Declaration",
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

int token_is_valid(Token *token) {
    return (token->kind != Token_Unknown) || (token->kind != Token_EOF);
}


int token_expected(Tokenizer *tokenizer, Token token, TokenKind token_kind) {
    if (token.kind != token_kind) {
        fprintf(
            stderr,
            "Error in line %d: Expected %s but got %s while reading %s\n",
            tokenizer->lines + 1, TypeNames[token_kind], 
            TypeNames[token.kind], token.info
        );
        exit(1);
        return 0;
    }
    return 1;
}


void accessors_push(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};
    GltfAccessor *accessor = arr_push(gltf->accessors, GltfAccessor);

    for (;
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_StrLiteral:
                if (strcmp(token.info, "bufferView") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    accessor->bufferView = atoi(token.info);
                }
                else if (strcmp(token.info, "count") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    accessor->count = atoi(token.info);
                }
                break;
            case Token_CloseCurl:
                return;
            default:
                break;
        }
    }
}


void handle_accessors(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenCurl:
                accessors_push(gltf, tokenizer);
                break;
            case Token_CloseBra:
                return;
            case Token_Coma:
                break;
            default:
                fprintf(
                    stderr, "Invalid Token on line %d: %s\n",
                    tokenizer->lines + 1, token.info
                );
                exit(1);
        }
    }
}

void bufferViews_push(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};
    GltfBufferView *buffer = arr_push(gltf->bufferViews, GltfBufferView);

    for (;
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_StrLiteral:
                if (strcmp(token.info, "byteLength") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->length = atoi(token.info);
                }
                else if (strcmp(token.info, "byteStride") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->stride = atoi(token.info);
                }
                else if (strcmp(token.info, "buffer") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->buffer = atoi(token.info);
                }
                else if (strcmp(token.info, "byteOffset") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->offset = atoi(token.info);
                }
                break;
            case Token_CloseCurl:
                return;
            default:
                break;
        }
    }
}


void handle_bufferViews(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenCurl:
                bufferViews_push(gltf, tokenizer);
                break;
            case Token_CloseBra:
                return;
            case Token_Coma:
                break;
            default:
                fprintf(
                    stderr, "Invalid Token on line %d: %s\n",
                    tokenizer->lines + 1, token.info
                );
                exit(1);
        }
    }
}

void meshes_push(GltfData *gltf, Tokenizer *tokenizer) {
    GltfMesh *buffer = arr_push(gltf->meshes, GltfMesh);


    int level = 1;

    for (
        Token token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_StrLiteral:
                if (strcmp(token.info, "JOINTS_0") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->joints = atoi(token.info);
                }
                else if (strcmp(token.info, "NORMAL") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->normal = atoi(token.info);
                }
                else if (strcmp(token.info, "POSITION") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->position = atoi(token.info);
                }
                else if (strcmp(token.info, "TEXCOORD_0") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->texcoord = atoi(token.info);
                }
                else if (strcmp(token.info, "indices") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->indices = atoi(token.info);
                }
                else if (strcmp(token.info, "WEIGHTS_0") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->weights = atoi(token.info);
                }
                break;
            case Token_OpenCurl:
                level++;
                break;
            case Token_CloseCurl:
                level--;
                if (level == 0) {
                    return;
                }
                break;
            default:
                break;
        }
    }
}


void handle_meshes(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenCurl:
                meshes_push(gltf, tokenizer);
                break;
            case Token_CloseBra:
                return;
            case Token_Coma:
                break;
            default:
                fprintf(
                    stderr, "Invalid Token on line %d: %s\n",
                    tokenizer->lines + 1, token.info
                );
                exit(1);
        }
    }
}

void buffers_push(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};
    GltfBuffer *buffer = arr_push(gltf->buffers, GltfBuffer);

    for (;
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_StrLiteral:
                if (strcmp(token.info, "byteLength") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Int);
                    
                    buffer->length = atoi(token.info);
                }
                else if (strcmp(token.info, "uri") == 0) {
                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_Colon);

                    token = token_next(tokenizer);
                    token_expected(tokenizer, token, Token_StrLiteral);
                    
                    strcpy(buffer->uri, token.info);
                }
                break;
            case Token_CloseCurl:
                return;
            default:
                break;
        }
    }
}


void handle_buffers(GltfData *gltf, Tokenizer *tokenizer) {
    Token token = {0};

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_OpenBra);

    for (
        token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenCurl:
                buffers_push(gltf, tokenizer);
                break;
            case Token_CloseBra:
                return;
            case Token_Coma:
                break;
            default:
                fprintf(
                    stderr, "Invalid Token on line %d: %s\n",
                    tokenizer->lines + 1, token.info
                );
                exit(1);
        }
    }
}

void skip_array(GltfData *gltf, Tokenizer *tokenizer) {
    int level = 1;

    for (
        Token token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenBra:
                level++;
                break;
            case Token_CloseBra:
                level--;
                if (level == 0) {
                    return;
                }
                break;
            default: 
                break;
        }
    }
}

void skip_struct(GltfData *gltf, Tokenizer *tokenizer) {
    int level = 1;

    for (
        Token token = token_next(tokenizer);
        tokenizer->data[tokenizer->cursor] && token_is_valid(&token);
        token = token_next(tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenCurl:
                level++;
                break;
            case Token_CloseCurl:
                level--;
                if (level == 0) {
                    return;
                }
                break;
            default: 
                break;
        }
    }
}

void handle_key_value_pair(GltfData *gltf, Tokenizer *tokenizer, Token token) {
    if (strcmp(token.info, "accessors") == 0) {
        handle_accessors(gltf, tokenizer);
        return;
    }
    if (strcmp(token.info, "buffers") == 0) {
        handle_buffers(gltf, tokenizer);
        return;
    }
    if (strcmp(token.info, "bufferViews") == 0) {
        handle_bufferViews(gltf, tokenizer);
        return;
    }
    if (strcmp(token.info, "meshes") == 0) {
        handle_meshes(gltf, tokenizer);
        return;
    }
    
    token = token_next(tokenizer);
    token_expected(tokenizer, token, Token_Colon);

    token = token_next(tokenizer);
    if (token.kind == Token_OpenCurl) {
        skip_struct(gltf, tokenizer);
        return;
    }
    if (token.kind == Token_OpenBra) {
        skip_array(gltf, tokenizer);
        return;
    }
    return;
}


GltfData parse_gltf_data(char *data) {
    GltfData gltf_data = {0};

    ArrayList *accessors = new_array_list(GltfAccessor); 
    ArrayList *buffers = new_array_list(GltfBuffer); 
    ArrayList *bufferViews = new_array_list(GltfBufferView); 
    ArrayList *meshes = new_array_list(GltfMesh); 

    gltf_data.accessors = accessors;
    gltf_data.buffers = buffers;
    gltf_data.bufferViews = bufferViews;
    gltf_data.meshes = meshes;


    Tokenizer tokenizer = {0};
    tokenizer.data = data;

    int parsing = 0;

    for (
        Token token = token_next(&tokenizer);
        tokenizer.data[tokenizer.cursor] && token_is_valid(&token);
        token = token_next(&tokenizer)
    ) {
        switch(token.kind) {
            case Token_OpenCurl:
                if (parsing != 0) {
                    fprintf(
                        stderr, "Error on line %d while reading %s\n",
                        tokenizer.lines + 1, token.info
                    );
                    exit(1);
                }
                parsing = 1;
                break;
            case Token_CloseCurl:
                if (parsing != 1) {
                    fprintf(
                        stderr, "Error on line %d while reading %s\n",
                        tokenizer.lines + 1, token.info
                    );
                    exit(1);
                }
                parsing = 1;
                break;
            case Token_StrLiteral:
                handle_key_value_pair(&gltf_data, &tokenizer, token);
                break;
            case Token_Coma:
                break;
            default:
                fprintf(
                    stderr, "Invalid Token on line %d: %s\n",
                    tokenizer.lines + 1, token.info
                );
                exit(1);
        };
    }

    return gltf_data;
}

GltfMesh *gltf_get_mesh(GltfData *gltf, int i) {
    
    GltfMesh *result = 0;
    if (gltf->meshes->counter > i) {
        result = &((GltfMesh *)gltf->meshes->data)[i];
    }
    return result;
}

GltfAccessor *gltf_get_accessors(GltfData *gltf, int i) {
    
    GltfAccessor *result = 0;
    if (gltf->accessors->counter > i) {
        result = &((GltfAccessor *)gltf->accessors->data)[i];
    }
    return result;
}

GltfBufferView *gltf_get_bufferViews(GltfData *gltf, int i) {
    
    GltfBufferView *result = 0;
    if (gltf->bufferViews->counter > i) {
        result = &((GltfBufferView *)gltf->bufferViews->data)[i];
    }
    return result;
}


typedef unsigned char uchar;

float load_float(char *data, unsigned int offset) {
    int i = 1;
    if ( *(char *)&i == 1) {
        return *(float *)&data[offset];
    }

    int b0 = 0xFF & data[offset];
    int b1 = 0xFF & data[offset + 1];
    int b2 = 0xFF & data[offset + 2];
    int b3 = 0xFF & data[offset + 3];

    i = b0 << 24 | b1 << 16 | b2 << 8 | b3;
    return *(float *)&i;
}

typedef unsigned int uint;

uint load_uint(char *data, unsigned int offset) {
    int i = 1;
    if ( *(char *)&i == 1) {
        return *(uint *)&data[offset];
    }

    int b0 = 0xFF & data[offset];
    int b1 = 0xFF & data[offset + 1];
    int b2 = 0xFF & data[offset + 2];
    int b3 = 0xFF & data[offset + 3];
    i = b0 << 24 | b1 << 16 | b2 << 8 | b3;
    return *(uint *)&i;
}



void gltf_load_attributes(
    GltfData *gltf, char *data, ArrayList *model_buffer, uint index, uint fac
) {
    GltfAccessor *accessors = gltf_get_accessors(gltf, index);
    GltfBufferView *bufferViews = gltf_get_bufferViews(
        gltf, accessors->bufferView
    );

    uint len = accessors->count * fac;
    uint offset = bufferViews->offset;

    for (int i=0; i<len; i++) {
        *arr_push(model_buffer, float) = load_float(data, offset + (i*4));
    }
}

void gltf_load_indices(
    GltfData *gltf, char *data, ArrayList *model_buffer, uint index
) {
    GltfAccessor *accessors = gltf_get_accessors(gltf, index);
    GltfBufferView *bufferViews = gltf_get_bufferViews(
        gltf, accessors->bufferView
    );

    uint len = accessors->count;
    uint offset = bufferViews->offset;

    for (int i=0; i<len; i++) {
        *arr_push(model_buffer, unsigned int) = (
            load_uint(data, offset + (i*4))
        );
    }
}


IntermediateModel load_data_from_gltf(GltfData *gltf, char *data) {
    ArrayList *vertices = new_array_list(float);
    ArrayList *normals = new_array_list(float);
    ArrayList *uvs = new_array_list(float);
    ArrayList *indices = new_array_list(unsigned int);

    GltfMesh *mesh = gltf_get_mesh(gltf, 0);
    gltf_load_attributes(gltf, data, vertices, mesh->position, 3);
    gltf_load_attributes(gltf, data, normals, mesh->normal, 3);
    gltf_load_attributes(gltf, data, uvs, mesh->texcoord, 2);
    gltf_load_indices(gltf, data, indices, mesh->indices);


    IntermediateModel result = {0};

    result.vertices = vertices->data;
    result.normals = normals->data;
    result.uvs = uvs->data;
    result.indices = indices->data;

    result.vertices_count = vertices->counter;
    result.normals_count = normals->counter;
    result.uvs_count = uvs->counter;
    result.indices_count = indices->counter;

    free(vertices);
    free(normals);
    free(uvs);
    free(indices);


    return result;
}


