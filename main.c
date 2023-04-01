/* Modified fom: https://learnopengl.com/Getting-started/Hello-Window */
#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "common.h"
#include "external/stb_truetype.h"
#include "graphics.h"
#include "mesh.h"
#include "utils/file_handler.h"


#define BAD_COORDS 1

float speed;
int entity_index;
int pulse_n;

float distance_from_player;


// TODO: Move to image.h
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
} Font;


Font font;


GlyphInfo getGlyphInfo(uint32_t character, float offsetX, float offsetY) {
    stbtt_aligned_quad quad;

    stbtt_GetPackedQuad(
        font.char_info,
        font.atlasWidth,
        font.atlasHeight,
        character - font.firstChar,
        &offsetX,
        &offsetY,
        &quad,
        1
    );
    float xmin = quad.x0;
    float xmax = quad.x1;
    float ymin = -quad.y1;
    float ymax = -quad.y0;

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


void initFont() {
    char *fontData = read_file("assets/fonts/VictorMono-Regular.ttf");
    U8 atlasData[font.atlasWidth * font.atlasHeight];

    stbtt_pack_context context;
    if (!stbtt_PackBegin(
        &context, atlasData, font.atlasWidth, font.atlasHeight, 0, 1, 0)
    ) {
        printf("Failed to initialize font");
        exit(1);
    }

    stbtt_PackSetOversampling(&context, font.oversampleX, font.oversampleY);
    if (!stbtt_PackFontRange(
        &context, (unsigned char *)fontData, 0, font.size, font.firstChar,
        font.charCount, font.char_info)
    ) {
        printf("Failed to pack font");
        exit(1);
    }

    stbtt_PackEnd(&context);

    glGenTextures(1, &font.texture);
    glBindTexture(GL_TEXTURE_2D, font.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, font.atlasWidth, font.atlasHeight,
        0, GL_RED, GL_UNSIGNED_BYTE, atlasData
    );
    // glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);
    log_if_err("Could not initialize font\n");
}


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);


int main() {
    font.size = 20;
    font.atlasWidth = 1024;
    font.atlasHeight = 1024;
    font.oversampleX = 2;
    font.oversampleY = 2;
    font.firstChar = ' ';
    font.charCount = '~' - ' ';
    font.texture = 0;


    speed = 0.5;
    entity_index = 0;
    pulse_n = 0;

    distance_from_player = 5.0;

    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(5.0, 0.0, 0.0);
    camera.centre = newVec3(0.0, 0.0, 0.0);
    camera.pitch = 0.1745;
    camera.yaw = 0.0;


    // mesh usage example
    Mesh mesh = {0};

    Vec3 mesh_vertices[64*64];
    Vec2 uvs[64*64];
    unsigned int mesh_indices[3*64*64];

    mesh.vertices = mesh_vertices;
    mesh.uvs = uvs;
    mesh.indices = mesh_indices;
    mesh_init(&mesh);


    BaseModel world_model = {0};
    load_data_to_model(
        &world_model, (float *) mesh.vertices, mesh.indices,
        3*sizeof(float)*mesh.vertices_len,
        sizeof(unsigned int)*mesh.indices_len
    );
    load_texture_to_model(
        &world_model,
        "assets/textures/marble-floor.jpg",
        // "assets/textures/wall.jpg",
        // "assets/textures/wood-floor.jpg",
        (float *)mesh.uvs, 
        2 * sizeof(float) * mesh.uvs_len
    );
    world_model.vertex_count = mesh.indices_len;





    char *text = "Find all things without being catch 0/10";
    log_if_err("Issue before Font initiation\n");
    initFont();
    log_if_err("Issue with Font initiation\n");

    Vec3 vertices[2000];
    Vec2 text_uvs[2000];
    unsigned int indexes[2000];

    float text_coord[8];


    uint16_t lastIndex = 0;
    float offsetX = 0, offsetY = 0;
    int counter = 0;
    int indices_counter = 0;
    char c;
    for (int i=0; i<strlen(text); i++) {
        c = text[i];
        GlyphInfo glyph_info = getGlyphInfo(c, offsetX, offsetY);
        offsetX = glyph_info.offsetX;
        offsetY = glyph_info.offsetY;

        vertices[counter] = glyph_info.positions[0];
        vertices[counter + 1] = glyph_info.positions[1];
        vertices[counter + 2] = glyph_info.positions[2];
        vertices[counter + 3] = glyph_info.positions[3];

        printf("POSITION 0: %f %f %f\n",
            glyph_info.positions[0].x,
            glyph_info.positions[0].y,
            glyph_info.positions[0].z
        );
        printf("POSITION 1: %f %f %f\n",
            glyph_info.positions[1].x,
            glyph_info.positions[1].y,
            glyph_info.positions[1].z
        );
        printf("POSITION 2: %f %f %f\n",
            glyph_info.positions[2].x,
            glyph_info.positions[2].y,
            glyph_info.positions[2].z
        );
        printf("POSITION 3: %f %f %f\n",
            glyph_info.positions[3].x,
            glyph_info.positions[3].y,
            glyph_info.positions[3].z
        );

        text_uvs[counter] = glyph_info.uvs[0];
        text_uvs[counter + 1] = glyph_info.uvs[1];
        text_uvs[counter + 2] = glyph_info.uvs[2];
        text_uvs[counter + 3] = glyph_info.uvs[3];


        text_coord[0] = glyph_info.uvs[0].x;
        text_coord[1] = glyph_info.uvs[0].y;

        text_coord[2] = glyph_info.uvs[1].x;
        text_coord[3] = glyph_info.uvs[1].y;

        text_coord[4] = glyph_info.uvs[2].x;
        text_coord[5] = glyph_info.uvs[2].y;

        text_coord[6] = glyph_info.uvs[3].x;
        text_coord[7] = glyph_info.uvs[3].y;
        


        printf("POSITION 0: %f %f \n",
            glyph_info.uvs[0].x,
            glyph_info.uvs[0].y
        );
        printf("POSITION 1: %f %f \n",
            glyph_info.uvs[1].x,
            glyph_info.uvs[1].y
        );
        printf("POSITION 2: %f %f \n",
            glyph_info.uvs[2].x,
            glyph_info.uvs[2].y
        );
        printf("POSITION 3: %f %f \n",
            glyph_info.uvs[3].x,
            glyph_info.uvs[3].y
        );


        indexes[indices_counter] = lastIndex;
        indexes[indices_counter + 1] = lastIndex + 1;
        indexes[indices_counter + 2] = lastIndex + 2;
        indexes[indices_counter + 3] = lastIndex;
        indexes[indices_counter + 4] = lastIndex + 2;
        indexes[indices_counter + 5] = lastIndex + 3;

        lastIndex += 4;
        counter += 4;
        indices_counter += 6;
    }
    printf("INDICES COUNTER: %d\n", indices_counter);
    printf("VERTICES COUNTER: %d\n", 3*counter);
    printf(" 4xlenx3: %d\n", (int)(4*strlen(text)*3));
    printf("UVs COUNTER: %d\n", 2*counter);
    printf(" 4xlenx2: %d\n", (int)(4*strlen(text)*2));
    log_if_err("Issue before vaos and  vbo\n");



    float vertices1[] = {
        -0.5f,-0.5f, 0.0f,
        -0.5f,0.5f, 0.0f,
        0.5f,0.5f, 0.0f,
        0.5f,-0.5f, 0.0f
    };

    float text_coord1[] = {
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f
	};

    unsigned int indices1[] = {
        0, 1, 2,
        0, 2, 3
    };

    BaseModel rect = {0};
    load_data_to_model(
        &rect, (float *)vertices1, indices1,
        3 * sizeof(float) * 4, sizeof(indices1)
    );
    rect.vertex_count = sizeof(indices1)/sizeof(indices1[0]);
    load_texture_to_model(
        // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
        &rect, "assets/textures/marble-floor.jpg", text_coord1, 
        sizeof(text_coord1)
    );

    BaseModel font_model = {0};
    load_data_to_model(
        &font_model, (float *)vertices, indexes,
        3*counter*sizeof(float), sizeof(unsigned int) * indices_counter
    );

    glBindVertexArray(font_model.vao);
    glEnableVertexAttribArray(1);

    store_float_in_attributes(
        &font_model.uv, 1, 2,
        2 * sizeof(float) * counter, (float *)text_uvs
    );
    font_model.vertex_count = indices_counter;
    font_model.texture_id = font.texture;


    BaseModel cube_model = {0};
    IntermediateModel cube_data = {0};
    parse_obj_file_simple("assets/models/cube.obj", &cube_data);
    load_data_to_model(
        &cube_model, cube_data.vertices, cube_data.indices,
        cube_data.vertices_count * sizeof(float),
        cube_data.indices_count * sizeof(unsigned int)
    );
    cube_model.vertex_count = cube_data.indices_count;
    load_texture_to_model(
        // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
        &cube_model, "assets/textures/wall.jpg", text_coord1, 
        sizeof(text_coord1)
    );


    BaseModel tea_model = {0};
    IntermediateModel tmp = {0};
    parse_obj_file_simple("assets/models/utah_teapot.obj", &tmp);
    load_data_to_model(
        &tea_model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    tea_model.vertex_count = tmp.indices_count;


    BaseModel suzanne = {0};
    IntermediateModel suzanne_data = {0};
    parse_obj_file("assets/models/suzanne.obj", &suzanne_data);
    load_data_to_model(
        &suzanne, suzanne_data.vertices, suzanne_data.indices,
        suzanne_data.vertices_count* sizeof(float),
        suzanne_data.indices_count * sizeof(unsigned int)
    );
    suzanne.vertex_count = suzanne_data.indices_count;

    Entity *entity = &renderer.entities[9];
    entity->model = &font_model;
    Vec3 entity_position_1 = newVec3(5, 0, -5);
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 0.05;

    entity = &renderer.entities[1];
    entity->model = &world_model;
    Vec3 entity_position_world = newVec3(0, 0, 0);
    entity->position = &entity_position_world;
    entity->active = 1;
    entity->scale = 1.0;

    // entity = &renderer.entities[1];
    // entity->model = &model;
    // Vec3 entity_position_2 = newVec3(0, 0, -20);
    // entity->position = &entity_position_2;
    // entity->active = 1;
    // entity->scale = 5.0;

    entity = &renderer.entities[0];
    entity->model = &cube_model;
    Vec3 entity_position_2 = newVec3(0, 0, 0);
    entity->position = &entity_position_2;
    entity->active = 1;
    entity->scale = 5.0;

    // entity = &renderer.entities[2];
    // entity->model = &suzanne;
    // Vec3 entity_position_3 = newVec3(10.0, 0.0, -50.0);
    // entity->position = &entity_position_3;
    // entity->active = 1;
    // entity->scale = 1.0;

    // entity = &renderer.entities[3];
    // entity->model = &tea_model;
    // Vec3 entity_position_4 = newVec3(10.0, 0.0, -30.0);
    // entity->position = &entity_position_4;
    // entity->active = 1;
    // entity->scale = 1.0;
    
    while (!glfwWindowShouldClose(ctx.window)) {
        printf("Entity SELETED: %d\n", entity_index);
        handle_input(&ctx, &renderer, &camera);

        render(&renderer, &camera);


        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera) {
    // glfwGetCursorPos(window, &xpos, &ypos);
    // double xpos, ypos;
    Entity *entity = &renderer->entities[entity_index];
    Entity *player = &renderer->entities[0];
    float d_player_move = 0.0;
    float rotation_factor = 0.1;

    if(
        glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_Q) == GLFW_PRESS
    ) {
        glfwSetWindowShouldClose(ctx->window, GL_TRUE);
    }

    if (glfwGetKey(ctx->window, GLFW_KEY_P ) == GLFW_PRESS){
        if ( (renderer->fill & (1<<1)) == 0) {
            renderer->fill |= (1<<1);
            renderer->fill ^= 1;
        }
    } else {
        renderer->fill &= ~(1<<1);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        increase_rotation(player, 0.0, rotation_factor * speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        increase_rotation(player, 0.0, -rotation_factor * speed, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        d_player_move += speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        d_player_move -= speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_N) == GLFW_PRESS ) {
        if (pulse_n == 0 ) {
            pulse_n = 1;
            entity_index++;
            if (entity_index > 3) {
                printf("%d", entity_index);
                entity_index = 0;
            }
        }
    }
    else {
        pulse_n = 0;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        printf("CONTROL PRESSED\n");
        if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {
            increase_rotation(entity, -speed, 0.0, 0.0);
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            printf("Button pressed\n");
            increase_rotation(entity, 0.0,speed, 0.0);
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            printf("Button pressed\n");
            increase_rotation(entity, speed, 0.0, 0.0);
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            printf("Button pressed\n");
            increase_rotation(entity, 0.0, -speed, 0.0);
        }
    }
    else {
        if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {
            // d_camera_move += speed;

            distance_from_player -= speed;  
            if (distance_from_player <= 0.5) {
                distance_from_player = 0.5;
            }
        }
        if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            distance_from_player += speed;
        }
    }


    if (glfwGetKey(ctx->window, GLFW_KEY_H) == GLFW_PRESS) {
        camera->yaw -= 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_L) == GLFW_PRESS) {
        camera->yaw += 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_J) == GLFW_PRESS) {
        camera->pitch -= 0.01;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_K) == GLFW_PRESS) {
        camera->pitch += 0.01;
    }

    player->position->x += d_player_move * sinf(player->rotation_y);
    player->position->z += d_player_move * cosf(player->rotation_y);

    float horizontal_distance = distance_from_player * cosf(camera->pitch);
    float vertical_distance = distance_from_player * sinf(camera->pitch);
    float theta = camera->yaw + player->rotation_y;

    camera->position.x = player->position->x - horizontal_distance*sinf(theta);
    camera->position.z = player->position->z - horizontal_distance*cos(theta);
    camera->position.y = player->position->y + vertical_distance;

    camera->centre = *player->position;

    printf("scale: %f\n", entity->scale);
    printf("pitch: %f\n", camera->pitch);
    printf("yaw: %f\n", camera->yaw);
}

