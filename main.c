#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "common.h"
#include "graphics.h"
#include "mesh.h"
#include "font.h"
#include "utils/file_handler.h"
#include "utils/helpers.h"


float speed;
int pulse_n;
int pulse_p;
int pulse_e;
float distance_from_player;
int show_debug_info;

int entity_index;
int entity_category_index;
char *entity_categories[3];

enum EntityCategory {
    EntityCategory_entities = 0,
    EntityCategory_gui = 1,
    EntityCategory_font = 2
};


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera);


int main() {
    Font font = {0};

    speed = 0.5;
    show_debug_info = 0;
    pulse_n = 0;
    pulse_e = 0;
    pulse_p = 0;
    distance_from_player = 5.0;

    entity_index = 0;
    entity_category_index = 0;
    entity_categories[0] = "Entity";
    entity_categories[1] = "GUI Entity";
    entity_categories[2] = "Font Entity";


    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(0.0, 5.0, -5.0);
    camera.centre = newVec3(0.0, 0.0, 0.0);
    camera.pitch = 0.1745;
    camera.yaw = 0.0;

    Light light = {0};
    light.position = newVec3(0.0, -1.0, 0.0);
    light.color = newVec3(0.8, 0.8, 1.0);

    renderer.light = &light;



    // mesh usage example
    Mesh mesh = {0};

    Vec3 mesh_vertices[64*64];
    Vec2 uvs[64*64];
    Vec3 mesh_normal[64*64];
    unsigned int mesh_indices[3*64*64];

    mesh.vertices = mesh_vertices;
    mesh.uvs = uvs;
    mesh.indices = mesh_indices;
    mesh.normal = mesh_normal;
    mesh_init(&mesh);


    BaseModel world_model = {0};
    load_data_to_model(
        &world_model, (float *) mesh.vertices, mesh.indices,
        3*sizeof(float)*mesh.vertices_len,
        sizeof(unsigned int)*mesh.indices_len
    );
    load_texture_to_model(
        &world_model,
        // "assets/textures/marble-floor.jpg",
        "assets/textures/wall.jpg",
        // "assets/textures/wood-floor.jpg",
        (float *)mesh.uvs, 
        2 * sizeof(float) * mesh.uvs_len
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(world_model.vao);
    store_float_in_attributes(
        &world_model.normal,
        2,
        3,
        3 * mesh.vertices_len * sizeof(float),
        (float *) mesh.normal
    );
    log_if_err("Issue after loading normals\n");

    world_model.vertex_count = mesh.indices_len;


    // float vertices1[] = {
    //     -0.5f,-0.5f, 0.0f,
    //     -0.5f,0.5f, 0.0f,
    //     0.5f,0.5f, 0.0f,
    //     0.5f,-0.5f, 0.0f
    // };
    
    float text_coord1[] = {
        0.0f,0.0f,
        0.0f,1.0f,
        1.0f,1.0f,
        1.0f,0.0f
	};

    // unsigned int indices1[] = {
    //     0, 1, 2,
    //     0, 2, 3
    // };

    // BaseModel rect = {0};
    // load_data_to_model(
    //     &rect, (float *)vertices1, indices1,
    //     3 * sizeof(float) * 4, sizeof(indices1)
    // );
    // rect.vertex_count = sizeof(indices1)/sizeof(indices1[0]);
    // load_texture_to_model(
    //     // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
    //     &rect, "assets/textures/marble-floor.jpg", text_coord1, 
    //     sizeof(text_coord1)
    // );



    log_if_err("Issue before Font initiation\n");
    float aspect_ratio = (float)ctx.width / (float)ctx.height;
    font_init(&font, "assets/fonts/VictorMono-Regular.ttf", aspect_ratio);
    renderer.font = &font;
    log_if_err("Issue with Font initiation\n");


    BaseModel cube_model = {0};
    IntermediateModel cube_data = {0};
    intermediate_model_init(&cube_data);
    parse_obj_file_simple("assets/models/cube.obj", &cube_data);
    load_data_to_model(
        &cube_model, cube_data.vertices, cube_data.indices,
        cube_data.vertices_count * sizeof(float),
        cube_data.indices_count * sizeof(unsigned int)
    );
    cube_model.vertex_count = cube_data.indices_count;
    intermediate_model_free(&cube_data);
    load_texture_to_model(
        // &rect, "assets/fonts/charmap-oldschool_white.png", text_coord1, 
        &cube_model, "assets/textures/wall.jpg", text_coord1, 
        sizeof(text_coord1)
    );

    BaseModel tea_model = {0};
    IntermediateModel tmp = {0};
    intermediate_model_init(&tmp);
    parse_obj_file_simple("assets/models/utah_teapot.obj", &tmp);
    load_data_to_model(
        &tea_model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    tea_model.vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);


    BaseModel suzanne = {0};
    IntermediateModel suzanne_data = {0};
    parse_obj_file("assets/models/suzanne.obj", &suzanne_data);
    // parse_obj_file("assets/models/dragon.obj", &suzanne_data);
    load_data_to_model(
        &suzanne, suzanne_data.vertices, suzanne_data.indices,
        suzanne_data.vertices_count* sizeof(float),
        suzanne_data.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        // &suzanne, "assets/textures/wall.jpg", suzanne_data.uvs, 
        &suzanne, "assets/textures/wood-floor.jpg", suzanne_data.uvs, 
        suzanne_data.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(suzanne.vao);
    store_float_in_attributes(
        &suzanne.normal,
        2,
        3,
        suzanne_data.normals_count * sizeof(float),
        suzanne_data.normals
    );
    log_if_err("Issue after loading normals\n");
    suzanne.vertex_count = suzanne_data.indices_count;
    intermediate_model_free(&suzanne_data);


    Mesh quad_mesh = {0};
    Vec3 vert[4];
    quad_mesh.vertices = vert;
    Vec2 uvs_coords[4];
    quad_mesh.uvs = uvs_coords;
    Vec3 normal[4];
    quad_mesh.normal = normal;
    Vec3 color[4];
    quad_mesh.color = color;
    unsigned int ind[10];
    quad_mesh.indices = ind;

    draw_quad(
        &quad_mesh, newVec3(0.5, 0.5, 0.0), newVec3(0.0, 0.0, 1.0), 0.5
    );
    BaseModel quad_model = {0};
    load_data_to_model(
        &quad_model, (float *)quad_mesh.vertices, quad_mesh.indices,
        3 * quad_mesh.vertices_len * sizeof(float),
        quad_mesh.indices_len * sizeof(unsigned int)
    );
    load_empty_texture_to_model(
        &quad_model, (float *)quad_mesh.uvs, 
        2 * quad_mesh.uvs_len * sizeof(float)
    );
    glBindVertexArray(quad_model.vao);
    store_float_in_attributes(
        &quad_model.color,
        2,
        3,
        3 * quad_mesh.color_len * sizeof(float),
        (float *)quad_mesh.color
    );
    log_if_err("Issue loading quad color\n");
    quad_model.vertex_count = quad_mesh.indices_len;


    Entity *entity = &renderer.font_entities[0];
    strcpy(entity->debug_name, "Default Font");
    entity->model = (BaseModel *) &font.vao;
    Vec3 entity_position_1 = newVec3(0.0, 0.0, 0.0);
    entity->position = &entity_position_1;
    entity->active = 1;
    entity->scale = 1;

    entity = &renderer.entities[1];
    strcpy(entity->debug_name, "World Floor");
    entity->model = &world_model;
    Vec3 entity_position_world = newVec3(0, 0, 0);
    entity->position = &entity_position_world;
    entity->active = 1;
    entity->scale = 1.0;

    entity = &renderer.gui_entities[0];
    strcpy(entity->debug_name, "Manual Quad");
    entity->model = &quad_model;
    Vec3 rect_pos = newVec3(0, 0, 0);
    entity->position = &rect_pos;
    entity->active = 1;
    entity->scale = 1.0;


    entity = &renderer.gui_entities[1];
    strcpy(entity->debug_name, "Automatic Quad");
    gui_quad_in_pos(
        &ctx, entity, newVec2(0.0, 0.0), 0.5, newVec3(1.0, 1.0, 0.0)
    );

    entity = &renderer.entities[0];
    strcpy(entity->debug_name, "Suzanne");
    entity->model = &suzanne;
    Vec3 entity_position_2 = newVec3(0, 1, 0);
    entity->position = &entity_position_2;
    entity->active = 1;
    entity->scale = 1.0;

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


Entity *get_entity_selected(Renderer *renderer) {
    if (entity_category_index == 0) {
        return &renderer->entities[entity_index];
    }
    else if (entity_category_index == 1) {
        return &renderer->gui_entities[entity_index];
    }
    else {
        return &renderer->font_entities[entity_index];
    }
}


void handle_debug_info(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera,
    double second_per_frame
) {
    float aspect_ratio = (float)ctx->width / (float)ctx->height;
    float font_aspect_ratio = renderer->font->base_x / renderer->font->base_y;

    char msg[500];
    sprintf(msg, "Distance from player (zoom): %0.3f", distance_from_player);
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "pitch: %0.3f", camera->pitch);
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "yaw: %.3f", camera->yaw);
    font_buffer_push(renderer->font, msg);

    sprintf(
        msg, "aspect ratio: %.3f, font_aspect_ratio: %.3f",
        aspect_ratio, font_aspect_ratio
    );
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "FPS: %.3f", 1.0/second_per_frame);
    font_buffer_push(renderer->font, msg);

    sprintf(
        msg, "Mouse x: %.3f, y: %.3f",
        ctx->mouse_position[0], ctx->mouse_position[1]
    );
    font_buffer_push_color(renderer->font, msg, newVec3(1.0, 0.0, 0.0));
    sprintf(
        msg, "%s Selected: %s",
        entity_categories[entity_category_index],
        get_entity_selected(renderer)->debug_name
    );
    font_buffer_push_color(renderer->font, msg, newVec3(1.0, 1.0, 0.0));
}


void camera_focus_movement(
    GraphicsContext *ctx, Camera *camera, float spf, float dist_from_p
) {
    CameraMovementParams camera_params = {0};
    camera_params.camera = camera;
    camera_params.speed = 0.05;
    camera_params.camera_speed = 1.0;
    camera_params.distance_from_player = dist_from_p;
    camera_params.dt = spf;

    free_camera_movement(ctx, &camera_params);

}
void player_focus_movement(
    GraphicsContext *ctx, Entity *player, Camera *camera,
    float spf, float dist_from_p
) {
    PlayerMovementParams params = {0};
    params.player = player;
    params.rotation_factor = 0.1;
    params.speed = 0.5;
    params.d_player_move = 0.0;
    params.dt = spf;
    player_movement(ctx, &params);

    CameraMovementParams camera_params = {0};
    camera_params.camera = camera;
    camera_params.speed = 0.05;
    camera_params.camera_speed = 1.0;
    camera_params.distance_from_player = dist_from_p;
    camera_params.dt = spf;

    camera_movement(ctx, &camera_params);
    distance_from_player = camera_params.distance_from_player;
    // camera_follow_player(player, &camera_params);
    camera_follow_player(player->position, player->rotation_y, &camera_params);
}


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera) {
    float prev_width = ctx->width;
    float prev_height = ctx->height;
    
    glfwGetWindowSize(ctx->window, &ctx->width, &ctx->height);
    if (prev_width != ctx->width || prev_height != ctx->height) {
        reload_projection_matrix(ctx, renderer);
    }

    double time = glfwGetTime();
    double second_per_frame = time - ctx->previous_time;
    ctx->previous_time = time;
    
    Entity *entity = get_entity_selected(renderer);
    Entity *player = &renderer->entities[0];

    if(
        glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_Q) == GLFW_PRESS
    ) {
        glfwSetWindowShouldClose(ctx->window, GL_TRUE);
    }

    if (toggle_button_press(ctx, GLFW_KEY_P, &pulse_p)) {
        entity->fill = 1 - entity->fill;
    }

    if (
        glfwGetKey(ctx->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
    ) {
        if (toggle_button_press(ctx, GLFW_KEY_N, &pulse_n)){
            entity_category_index++;
            entity_index = 0;
            if (entity_category_index > 2) {
                entity_category_index = 0;
            }
        }

    }
    else if (toggle_button_press(ctx, GLFW_KEY_N, &pulse_n)){
        entity_index++;
        if (get_entity_selected(renderer)->active == 0) {
            entity_index = 0;
        }
    }

    if (toggle_button_press(ctx, GLFW_KEY_E, &pulse_e)){
        show_debug_info = 1 - show_debug_info;
    }

    camera_focus_movement(
        ctx, camera, second_per_frame, distance_from_player
    );
    // player_focus_movement(
    //     ctx, player, camera, second_per_frame, distance_from_player
    // );

    float aspect_ratio = (float)ctx->width / (float)ctx->height;
    font_buffer_reset(renderer->font, aspect_ratio);
    if (show_debug_info) {
        handle_debug_info(ctx, renderer, camera, second_per_frame);
    }
}



