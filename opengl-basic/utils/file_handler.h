#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H


#define VerticesCapacity 20000
#define NormalsCapacity 20000
#define TexCapacity 20000
#define IndicesCapacity 20000
#define BufferCapacity 500


typedef struct IntermediateModel {
    float *vertices;
    float *normals;
    float *uvs;
    unsigned int *indices; 
    unsigned int vertices_count;
    unsigned int normals_count;
    unsigned int uvs_count;
    unsigned int indices_count;
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
void intermediate_model_init(IntermediateModel *model);
void intermediate_model_free(IntermediateModel *model);
void transform_obj_file(char *file_path, char *file_output);


#endif  // FILE_HANDLER_H