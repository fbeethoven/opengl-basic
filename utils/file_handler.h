#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H


#define VerticesCapacity 20000
#define IndicesCapacity 20000
#define BufferCapacity 500


typedef struct IntermediateModel {
    float vertices[VerticesCapacity];
    unsigned int indices[IndicesCapacity]; 
    int vertices_count;
    int indices_count;
} IntermediateModel;


typedef struct StrSplitter {
    int cursor;
    char separator;
    char buffer[BufferCapacity];
} StrSplitter;


char *read_file(char *file_path);
void parse_obj_file_simple(char *data, IntermediateModel *dest);
void parse_obj_file(char *data, IntermediateModel *dest);
int split_next(StrSplitter *splitter, char *data);


#endif  // FILE_HANDLER_H
