#include <stdio.h>
#include <stdlib.h>

#include "file_handler.h"


typedef struct IntermediateModel {
    float vertices[5000];
    unsigned int indices[5000]; 
    int vertices_count;
    int indices_count;
} IntermediateModel;

static char *read_file(char *file_path) {
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


void read_line(const char *data, IntermediateModel *dest) {
    for (const char *i=data; *i && *i != '\n'; i++) {
        if (*i == 'v' && i[1] == ' ') {
            get_vertices();
        }
        else {
            continue;
        }
    }
}

int main() {
    char *data = read_file("assets/models/utah_teapot.obj");
    IntermediateModel tmp = {0};
    read_line(data, &tmp);
    // printf("%s\n", line);

    printf("Hello World\n");
}
