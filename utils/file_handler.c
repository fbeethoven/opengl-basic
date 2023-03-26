#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_handler.h"


int push_vertice(IntermediateModel *dest, float x) {
    if (dest->vertices_count == VerticesCapacity) {
        return -1;
    }
    dest->vertices[dest->vertices_count++] = x;
    return 0;
}


int push_index(IntermediateModel *dest, unsigned int x) {
    printf("Pushing: %d\n", x);
    if (dest->indices_count == IndicesCapacity) {
        return -1;
    }
    dest->indices[dest->indices_count++] = x;
    return 0;
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
    splitter->cursor++;
    if (!data[splitter->cursor]) {
        return -2;
    }
    return 0;
}


void splitter_reset(StrSplitter *splitter) {
    splitter->cursor = 0;
    memset(splitter->buffer, 0, BufferCapacity * sizeof(char));
}


char *read_file(char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Failed to read input\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    char *data = malloc(sizeof(char) * file_size);
    if(fread(data, sizeof(char), file_size, file) != file_size) {
        fprintf(stderr, "There was an error reading input\n");
        free(data);
        fclose(file);
        exit(1);
    }

    fclose(file);
    return data;
}



void parse_obj_file(char *file_path, IntermediateModel *dest) {
    char *data = read_file(file_path);
    StrSplitter split_line = {0};
    split_line.separator = '\n';
    StrSplitter split_space = {0};
    split_space.separator = ' ';

    while (split_next(&split_line, data) >= 0) {
        if (split_line.buffer[0] == 'v') {
            splitter_reset(&split_space);

            split_next(&split_space, split_line.buffer);
            split_next(&split_space, split_line.buffer);
            push_vertice(dest, atof(split_space.buffer));
            split_next(&split_space, split_line.buffer);
            push_vertice(dest, atof(split_space.buffer));
            split_next(&split_space, split_line.buffer);
            push_vertice(dest, atof(split_space.buffer));
        }
        else if ( split_line.buffer[0] == 'f') {
            splitter_reset(&split_space);

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

