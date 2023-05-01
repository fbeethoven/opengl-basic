#ifndef FONT_H
#define FONT_H


#include "common.h"
#include "stb_truetype.h"
#include "mesh.h"
#include "utils/file_handler.h"


#define FontCapacity 20000



typedef struct GlyphInfo {
    Vec3 positions[4];
    Vec2 uvs[4];
    float offsetX, offsetY;
} GlyphInfo;


typedef struct Font {
    U32 size;
    U32 atlasWidth;
    U32 atlasHeight;
    U32 oversampleX;
    U32 oversampleY;
    U32 firstChar;
    U32 charCount;
    stbtt_packedchar char_info[127];
    U32 texture;
    float width;
    float height;


    Mesh *font_mesh;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
    unsigned int uv;
    unsigned int color_buffer;
    int vertex_count;
    unsigned int texture_id;

    float y_step;
    float base_x;
    float base_y;
} Font;


GlyphInfo getGlyphInfo(
    Font *font, uint32_t character, float offsetX, float offsetY
);

void font_init(Font *font, char *font_file_path, float width, float height);
void font_buffer_reset(Font *font, float width, float height);
Vec2 font_buffer_push(Font *font, char *msg);
Vec2 font_buffer_push_color(Font *font, char *msg, Vec3 color);
Vec2 _font_buffer_push(Font *font, char *msg, Vec2 position, Vec3 color);
void font_free(Font *font);
void font_update_buffer(Font *font);


#endif  // FONT_H
