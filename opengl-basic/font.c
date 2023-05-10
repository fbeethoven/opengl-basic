#include "font.h"


GlyphInfo getGlyphInfo(
    Font *font, uint32_t character, float offsetX, float offsetY
) {
    stbtt_aligned_quad quad;

    stbtt_GetPackedQuad(
        font->char_info,
        font->atlasWidth,
        font->atlasHeight,
        character - font->firstChar,
        &offsetX,
        &offsetY,
        &quad,
        1
    );
    float xmin = 2*quad.x0/font->width - 1.0;
    float xmax = 2*quad.x1/font->width - 1.0;
    float ymin = 1.0 -2*quad.y1/font->height;
    float ymax = 1.0 -2*quad.y0/font->height;

    GlyphInfo info = {0};
    info.offsetX = offsetX;
    info.offsetY = offsetY;
    info.positions[0] = newVec3(xmin, ymin, 0.0);
    info.positions[1] = newVec3(xmin, ymax, 0.0);
    info.positions[2] = newVec3(xmax, ymax, 0.0);
    info.positions[3] = newVec3(xmax, ymin, 0.0);
    info.uvs[0] = newVec2(quad.s0, quad.t1);
    info.uvs[1] = newVec2(quad.s0, quad.t0);
    info.uvs[2] = newVec2(quad.s1, quad.t0);
    info.uvs[3] = newVec2(quad.s1, quad.t1);

    return info;
}


Mesh *font_mesh_new() {
    Mesh *font_mesh = (Mesh *) malloc(sizeof(Mesh));
    Vec3 *vertices = (Vec3 *) malloc(FontCapacity * sizeof(Vec3));
    font_mesh->vertices = vertices;
    Vec2 *text_uvs = (Vec2 *) malloc(FontCapacity * sizeof(Vec2));
    font_mesh->uvs = text_uvs;
    Vec3 *color = (Vec3 *) malloc(FontCapacity * sizeof(Vec3));
    font_mesh->color = color;
    unsigned int *indexes = (
        (unsigned int *) malloc(FontCapacity * sizeof(unsigned int))
    );
    font_mesh->indices = indexes;

    return font_mesh;
}

void font_mesh_free(Mesh *font_mesh) {
    free(font_mesh->vertices);
    free(font_mesh->uvs);
    free(font_mesh->color);
    free(font_mesh->indices);
    free(font_mesh);
}


void font_init(Font *font, char *font_file_path, float width, float height) {
    font->size = 25;
    font->atlasWidth = 1024;
    font->atlasHeight = 1024;
    font->oversampleX = 2;
    font->oversampleY = 2;
    font->firstChar = ' ';
    font->charCount = '~' - ' ';
    font->texture = 0;

    font->y_step = 20.0;
    font->base_x = 0.0;
    font->base_y = 0.0;

    font->width = width;
    font->height = height;


    glGenVertexArrays(1, &font->vao);
    glBindVertexArray(font->vao);
    log_if_err("Font: vertex initiation\n");
    

    char *fontData = read_file(font_file_path);
    // U8 atlasData[font->atlasWidth * font->atlasHeight];
    U8* atlasData = (U8*)malloc(font->atlasWidth * font->atlasHeight);

    stbtt_pack_context context;
    if (!stbtt_PackBegin(
        &context, atlasData, font->atlasWidth, font->atlasHeight, 0, 1, 0)
    ) {
        printf("Failed to initialize font");
        exit(1);
    }

    stbtt_PackSetOversampling(&context, font->oversampleX, font->oversampleY);
    if (!stbtt_PackFontRange(
        &context, (unsigned char *)fontData, 0, font->size, font->firstChar,
        font->charCount, font->char_info)
    ) {
        printf("Failed to pack font");
        exit(1);
    }

    stbtt_PackEnd(&context);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &font->texture);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, font->atlasWidth, font->atlasHeight,
        0, GL_RED, GL_UNSIGNED_BYTE, atlasData
    );
    // glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(atlasData);
    log_if_err("Could not initialize font\n");

    font->font_mesh = font_mesh_new();


    glBindVertexArray(font->vao);
    glGenBuffers(1, &font->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->ibo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        FontCapacity,
        0,
        GL_DYNAMIC_DRAW
    );

    glGenBuffers(1, &font->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        FontCapacity,
        0,
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenBuffers(1, &font->uv);
	glBindBuffer(GL_ARRAY_BUFFER, font->uv);
    glBufferData(
        GL_ARRAY_BUFFER,
        FontCapacity,
        0,
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(
        1, 2, GL_FLOAT,
        GL_FALSE, 2 * sizeof(float), 0
    );
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &font->color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, font->color_buffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        FontCapacity,
        0,
        GL_DYNAMIC_DRAW
    );
    glVertexAttribPointer(
        2, 3, GL_FLOAT,
        GL_FALSE, 3 * sizeof(float), 0
    );
	glBindBuffer(GL_ARRAY_BUFFER, 0);


    font->vertex_count = font->font_mesh->indices_len;
    font->texture_id = font->texture;
}


void font_buffer_reset(Font *font, float width, float height) {
    font->font_mesh->vertices_len = 0;
    font->font_mesh->uvs_len = 0;
    font->font_mesh->color_len = 0;
    font->font_mesh->indices_len = 0;

    font->y_step = 20.0;
    font->base_x = 0.0;
    font->base_y = 0.0;

    font->width = width;
    font->height = height;
}


void font_update_buffer(Font *font) {
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        font->font_mesh->vertices_len * 3 * sizeof(float),
        (float *)font->font_mesh->vertices
    );
    glBindBuffer(GL_ARRAY_BUFFER, font->uv);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        font->font_mesh->uvs_len * 2 * sizeof(float),
        (float *)font->font_mesh->uvs
    );
    glBindBuffer(GL_ARRAY_BUFFER, font->color_buffer);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        font->font_mesh->color_len * 3 * sizeof(float),
        (float *)font->font_mesh->color
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->ibo);
    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        font->font_mesh->indices_len * 2 * sizeof(float),
        (unsigned int *)font->font_mesh->indices
    );
    log_if_err("Issue with subdata\n");

}


Vec2 _font_buffer_push(Font *font, char *msg, Vec2 position, Vec3 color) {
    Vec2 size = {0};
    Mesh *mesh = font->font_mesh;
    float offsetX = position.x;
    float offsetY = position.y;
    int counter = mesh->vertices_len;
    int indices_counter = mesh->indices_len;
    char c;
    for (int i=0; i<strlen(msg); i++) {
        c = msg[i];
        GlyphInfo glyph_info = getGlyphInfo(font, c, offsetX, offsetY);
        offsetX = glyph_info.offsetX;
        offsetY = glyph_info.offsetY;

        mesh->vertices[counter] = glyph_info.positions[0];
        mesh->vertices[counter + 1] = glyph_info.positions[1];
        mesh->vertices[counter + 2] = glyph_info.positions[2];
        mesh->vertices[counter + 3] = glyph_info.positions[3];

        mesh->uvs[counter] = glyph_info.uvs[0];
        mesh->uvs[counter + 1] = glyph_info.uvs[1];
        mesh->uvs[counter + 2] = glyph_info.uvs[2];
        mesh->uvs[counter + 3] = glyph_info.uvs[3];

        mesh->color[counter] = color;
        mesh->color[counter + 1] = color;
        mesh->color[counter + 2] = color;
        mesh->color[counter + 3] = color;

        mesh->indices[indices_counter] = counter;
        mesh->indices[indices_counter + 1] = counter + 1;
        mesh->indices[indices_counter + 2] = counter + 2;
        mesh->indices[indices_counter + 3] = counter;
        mesh->indices[indices_counter + 4] = counter + 2;
        mesh->indices[indices_counter + 5] = counter + 3;

        counter += 4;
        indices_counter += 6;

        if (glyph_info.positions[2].y > size.y) {
            size.y = glyph_info.positions[2].y;
        }
        size.x = glyph_info.positions[2].x;
    }

    mesh->vertices_len = counter;
    mesh->uvs_len = counter;
    mesh->color_len = counter;
    mesh->indices_len = indices_counter;


    return newVec2(
        (0.5 + 0.5*size.x) * font->width - position.x,
        position.y - (0.5 - 0.5*size.y) * font->height
    );
}


Vec2 font_buffer_push(Font *font, char *msg) {
    Vec2 result = _font_buffer_push(
        font, msg, newVec2(font->base_x, font->base_y), newVec3(1.0, 1.0, 1.0)
    );
    font->base_y += font->y_step;
    return result;
}


Vec2 font_buffer_push_color(Font *font, char *msg, Vec3 color) {
    Vec2 result = _font_buffer_push(
        font, msg, newVec2(font->base_x, font->base_y), color
    );
    font->base_y += font->y_step;
    return result;
}


void font_free(Font *font) {
    free(font->font_mesh->vertices);
    free(font->font_mesh->uvs);
    free(font->font_mesh->color);
    free(font->font_mesh->indices);
    free(font->font_mesh);
}

