#define GLAD_GL_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "packman.h"
#include "../mesh.h"
#include "../font.h"
#include "../utils/file_handler.h"
#include "../utils/helpers.h"


float player_rotation;

int game_run() {
    // TODO: 
    //  [X] add color to textures (gamma correction)
    //  [ ] gui buttons
    //  [ ] collision: AABB or SAT?
    //  [ ] rotate entity towards target point
    //  [ ] camera movement using mouse
    //  [ ] gui sliders
    //  [ ] quad sprite facing the camera
    //  [ ] mouse picker
    //  [ ] debug random segfault: maybe in floor textures?
    //      - confirm it is in floor

    GameContext g_ctx = {0};
    game_ctx = &g_ctx;
    rand_init(game_ctx);

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
    sync_entities(game_ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(0.0, 60.0, -145.0);
    camera.centre = newVec3(0.0, 55.0, -140.0);
    camera.pitch = 0.6;
    camera.yaw = 0.0;

    Light light = {0};
    light.position = newVec3(0.0, -1.0, 0.0);
    light.color = newVec3(0.8, 0.8, 1.0);

    renderer.light = &light;

    load_assets(&ctx, &renderer, game_ctx, &font);

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
    char msg[500];
    sprintf(
        msg, "FPS: %.3f | %.3f ms", 1.0/second_per_frame, second_per_frame
    );
    font_buffer_push(renderer->font, msg);

    sprintf(
        msg, "%s Selected: %s",
        entity_categories[entity_category_index],
        get_entity_selected(renderer)->debug_name
    );
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "Current Time: %f", game_ctx->current_time);
    font_buffer_push_color(renderer->font, msg, newVec3(1.0, 1.0, 0.0));

    font_buffer_push_color(renderer->font, "Camera:", newVec3(1.0, 1.0, 0.0));
    sprintf(
        msg, " Position: %f %f %f",
        camera->position.x, camera->position.y, camera->position.z
    );
    font_buffer_push_color(renderer->font, msg, newVec3(1.0, 1.0, 0.0));
    sprintf(
        msg, " Center: %f %f %f",
        camera->centre.x, camera->centre.y, camera->centre.z
    );
    font_buffer_push_color(renderer->font, msg, newVec3(1.0, 1.0, 0.0));
    sprintf(
        msg, " Yaw: %f | Pitch: %f",
        camera->yaw, camera->pitch
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
    camera_params.player_rotation = player_rotation;

    free_camera_movement(ctx, &camera_params);
    player_rotation = camera_params.player_rotation;

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

    update_entities(game_ctx);
    int new_rand = get_new_random(game_ctx);
    if (new_rand) {
        add_random_entity(ctx, game_ctx);
        add_random_entity(ctx, game_ctx);
    }
    
    Entity *entity = get_entity_selected(renderer);

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

    float aspect_ratio = (float)ctx->width / (float)ctx->height;
    font_buffer_reset(renderer->font, aspect_ratio);
    if (show_debug_info) {
        handle_debug_info(ctx, renderer, camera, second_per_frame);
    }
}

