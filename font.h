#ifndef FONT_H
#define FONT_H


#include "common.h"
#include "stb_truetype.h"
#include "mesh.h"
#include "utils/file_handler.h"



typedef struct GlyphInfo {
    Vec3 positions[4];
    Vec2 uvs[4];
    float offsetX, offsetY;
} GlyphInfo;


typedef struct Font{
    U32 size;
    U32 atlasWidth;
    U32 atlasHeight;
    U32 oversampleX;
    U32 oversampleY;
    U32 firstChar;
    U32 charCount;
    stbtt_packedchar char_info[127];
    U32 texture;


    Mesh *font_mesh;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
    unsigned int uv;
    int vertex_count;
    unsigned int texture_id;
} Font;


GlyphInfo getGlyphInfo(
    Font *font, uint32_t character, float offsetX, float offsetY
);

void font_init(Font *font, char *font_file_path);
void font_buffer_reset(Font *font);
void font_buffer_push(Font *font, char *msg, Vec2 position);


#endif  // FONT_H
