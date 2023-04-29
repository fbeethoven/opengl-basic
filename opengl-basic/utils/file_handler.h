#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "../memory.h"

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


typedef enum TokenKind {
    Token_Unknown,
    Token_StrLiteral,
    Token_Colon,
    Token_Coma,
    Token_Int,
    Token_Float,
    Token_Bool,
    Token_OpenCurl,
    Token_CloseCurl,
    Token_OpenBra,
    Token_CloseBra,
    Token_EOF,

    Token_Count
} TokenKind;

typedef struct Token {
    TokenKind kind;
    char info[256];
} Token;


typedef struct GltfAccessor {
    unsigned int bufferView;
    unsigned int count;
} GltfAccessor;


typedef struct GltfBufferView {
    unsigned int buffer;
    unsigned int length;
    unsigned int stride;
    unsigned int offset;
} GltfBufferView;

typedef struct GltfBuffer {
    unsigned int length;
    char uri[64];
} GltfBuffer;

typedef struct GltfMesh {
    unsigned int normal;
    unsigned int position;
    unsigned int texcoord;

    unsigned int indices;

    unsigned int weights;
    unsigned int joints;
} GltfMesh;

typedef struct GltfData {
    ArrayList *accessors; 
    ArrayList *buffers; 
    ArrayList *bufferViews; 
    ArrayList *meshes;
} GltfData;

typedef struct JsonKey JsonKey;
struct JsonKey {
    JsonKey *next;
    char key[64];
};

typedef struct JsonObj {
    JsonKey *first;
    JsonKey *last;
} JsonObj;


typedef enum JsonValueKind {
    JValueKind_Int,
    JValueKind_Float,
    JValueKind_Bool,
    JValueKind_String,
    JValueKind_JsonObj,
    JValueKind_Array,

    JValueKind_Count,
} JsonValueKind;


typedef struct JsonValue JsonValue;
struct JsonValue {
    JsonValueKind kind;
    JsonKey *key;
    JsonValue *next;
    JsonValue *hahs_next;

    union {
        int value_i;
        float value_f;
        char value_s[50];
        JsonObj *value_j;
    };
};


char *read_file(char *file_path);
void parse_obj_file_simple(char *data, IntermediateModel *dest);
void parse_obj_file(char *data, IntermediateModel *dest);
int split_next(StrSplitter *splitter, char *data);
void intermediate_model_init(IntermediateModel *model);
void intermediate_model_free(IntermediateModel *model);
void transform_obj_file(char *file_path, char *file_output);
GltfData parse_gltf_data(char *data);


#endif  // FILE_HANDLER_H
