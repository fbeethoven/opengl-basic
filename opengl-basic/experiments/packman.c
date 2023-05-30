#define GLAD_GL_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION


#include "packman.h"
#include "../mesh.h"
#include "../font.h"
#include "../utils/file_handler.h"
#include "../utils/helpers.h"
#include "../memory.h"

#include "../audio.h"


float player_rotation;
int random_experiment;
int pulse_r;
int stage;


typedef struct TestNode TestNode;

struct TestNode {
    int data;
};

LIST_ADD(TestNode);


int clean_up();
int game_run1();


int game_run() {

    sound_init();
    return game_run1();
}

int clean_up() {
    // TODO(CLEAN UP):
    // [X] Lists with types
    // [X] Clean warnings
    // [ ] Remove experiment helper
    //      [ ] add utils/random module
    //      [ ] move entity helpers and load assets to graphics module
    //      [X] move animation params to animation module
    //          (including debug options)
    // [X] Fix Entity Struct
    // [ ] Fix Render Struct
    //      [ ] Use list for entities first.
    //          In the future use a layer stack with callbacks
    // [ ] Add Layers
    // [ ] Improve Font rendereing
    // [ ] Improve UI (double buffering vs full ImGui)
    // [ ] Event System (Maybe we just want a toggle?)
    // [ ] Remove all debug printing


    // TODO(Weekend):
    // [X] Add sound (not really part of clean up but I 


    printf("Everything is working\n");
    return 0;

}


int game_run1() {
    // TODO:
    //  [X] add color to textures (gamma correction)
    //  [X] gui buttons
    //  [ ] gui sliders
    //  [ ] collision:
    //      [ ] AABB
    //      [ ] SAT
    //      [X] Spherical
    //  [X] add gravity
    //  [ ] rotate entity towards target point
    //  [X] camera movement using mouse
    //  [ ] quad sprite facing the camera
    //  [X] mouse picker
    //      [ ] Fix issue with camera rotations
    //  [X] debug random segfault: maybe in floor textures?
    //      - confirm it is in floor
    //  [X] sphere visualizer
    //  [ ] OBJ Parser Update:
    //      [ ] normalize normal vectors
    //      [ ] handle EOF
    //  [X] Fix file_handler for big endian


    GameContext g_ctx = {0};
    game_ctx = &g_ctx;
    rand_init(game_ctx);

    Font font = {0};

    speed = 0.5;
    player_is_grounded = 0;
    show_debug_info = 0;
    pulse_n = 0;
    pulse_e = 0;
    pulse_p = 0;
    pulse_r = 0;
    stage = -1;
    distance_from_player = 5.0;
    random_experiment = 0;

    // entity_index = 10;
    entity_index = 0;
    entity_category_index = 0;
    entity_categories[0] = "Entity";
    entity_categories[1] = "GUI Entity";
    entity_categories[2] = "Font Entity";


    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }
    glfwSetInputMode(ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);
    // sync_entities(game_ctx, &renderer);

    Camera camera = {0};

    camera.position = newVec3(CAMERA_POSITION);
    camera.centre = newVec3(CAMERA_CENTRE);
    camera.pitch = CAMERA_PITCH;
    camera.yaw = CAMERA_YAW;

    Light light = {0};
    light.position = newVec3(LIGHT_DIR);
    light.color = newVec3(LIGHT_COLOR);

    renderer.light = &light;

    load_assets(&ctx, &renderer, game_ctx, &font, &camera);

    while (!glfwWindowShouldClose(ctx.window)) {
        handle_input(&ctx, &renderer, &camera);

        render(&renderer, &camera);

        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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
    camera_params.speed = 0.0;
    camera_params.camera_speed = speed;
    camera_params.distance_from_player = dist_from_p;
    camera_params.dt = spf;
    camera_params.player_rotation = player_rotation;
    camera_params.player_is_grounded = player_is_grounded;

    free_camera_movement(ctx, &camera_params);
    player_rotation = camera_params.player_rotation;
    player_is_grounded = camera_params.player_is_grounded;

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
    camera_follow_player(
        &player->position, player->rotation.y, &camera_params
    );
}


void update_graphic_state(GraphicsContext *ctx, Renderer *renderer) {
    float prev_width = ctx->width;
    float prev_height = ctx->height;
    float FOV = renderer->FOV;
    if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS){
        FOV -= 0.001;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS){
        FOV += 0.001;
    }
    glfwGetWindowSize(ctx->window, &ctx->width, &ctx->height);
    if (
        prev_width != ctx->width || prev_height != ctx->height ||
        renderer->FOV != FOV
    ) {
        renderer->FOV = FOV;
        reload_projection_matrix(ctx, renderer);
    }
    reload_projection_matrix(ctx, renderer);

    double prev_x = ctx->mouse_position[0];
    double prev_y = ctx->mouse_position[1];
    glfwGetCursorPos(
        ctx->window, &ctx->mouse_position[0], &ctx->mouse_position[1]
    );
    ctx->dmouse[0] = ctx->mouse_position[0] - prev_x;
    ctx->dmouse[1] = ctx->mouse_position[1] - prev_y;
}


void handle_input(GraphicsContext *ctx, Renderer *renderer, Camera *camera) {
    update_graphic_state(ctx, renderer);

    double time = glfwGetTime();
    double second_per_frame = time - ctx->current_time;
    ctx->current_time = time;
    game_ctx->current_time = time;


    Entity *entity;


    // mouse picking
    // entity = &renderer->entities[10];
    // Vec3 position = mouse_to_plane( ctx, renderer, camera, newVec3(0.0, 1.0, 0.0), 0.0);
    // *entity->position = mouse_to_plane( ctx, renderer, camera, newVec3(0.0, 1.0, 0.0), 0.0);



    // if ( (time - game_ctx->start_time) > 5.0 && stage < 0) {
    //     for (int i=1; i<10; i++) {
    //         entity = &renderer->entities[i];
    //         entity->active = 0;
    //     }
    //     random_experiment = 1;
    //     stage = 0;
    // }
    // if (entity->active != 0) {
    //     float freq = 1.0;
    //     float pulse = (float)time - (int)time;
    //     pulse = sin(3.1415 * pulse * freq);
    //     pulse *= pulse;

    //     entity->color = vec3_lerp(
    //         newVec3(1.0, 0.0, 0.0), newVec3(1.0, 1.0, 0.0), pulse
    //     );
    // }


    // if (random_experiment) {
    //     update_entities(game_ctx, camera);
    //     if (time - game_ctx->prev_rand_time > 3.0) {
    //         game_ctx->prev_rand_time = time;
    //         add_random_entity(ctx, game_ctx, camera);
    //     }
    // }

    if(
        glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_Q) == GLFW_PRESS
    ) {
        glfwSetWindowShouldClose(ctx->window, GL_TRUE);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_X) == GLFW_PRESS) {
        camera_reset(camera);
    }

    if (toggle_button_press(ctx, GLFW_KEY_E, &pulse_e)){
        show_debug_info = 1 - show_debug_info;
        if (show_debug_info) {
            glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (toggle_button_press(ctx, GLFW_KEY_R, &pulse_r)){
        game_ctx->start_time = time;
        camera->position = newVec3(0.0, 60.0, 0.0);
        game_ctx->game_over = 0;
        player_is_grounded = 0;
        camera->pitch = 2.2;
        camera->yaw = 1.57;
        // random_experiment = 1 - random_experiment;
    }

    if (!show_debug_info) {
        camera_focus_movement(
            ctx, camera, second_per_frame, distance_from_player
        );
    }
    if (camera->position.y <= -20.0) {
        game_ctx->game_over = 1;
    }


    font_buffer_reset(renderer->font, (float)ctx->width, (float)ctx->height);
    if (show_debug_info) {
        float button_x_pos = 50.0;
        float button_y_pos = 50.0;
        float button_step = 25.0;

        entity = &LIST_GET(renderer->entities, 1);
        char *button_text = entity->fill ? "Wiremesh: ON" : "Wiremesh: OFF";
        if (ui_button(
            ctx, renderer, newVec2(button_x_pos, button_y_pos), button_text
        )) {
            if (pulse_p == 0) {
                pulse_p = 1;
            }
        }
        else {
            if (pulse_p == 1) {
                entity->fill = 1 - entity->fill;
                pulse_p = 0;
            }
        }

        button_y_pos += 2*button_step;
        button_text = play_audio ? "Sound Test: On" : "Sound Test: Off";
        if (ui_button(
            ctx, renderer, newVec2(button_x_pos, button_y_pos), button_text)
        ) {
            if (play_audio == 0) {
                last_play = time;
                play_sound();
            }
        }
        else {
            if (time - last_play >= 1.0) {
                play_audio = 0;
            }
        }
    }
    else {
        // entity = &LIST_GET(renderer->gui_entities, 0);
        // entity->active = 0;
    }

    // char msg[500];
    // int timer = 3 + (int)game_ctx->start_time - (int)time;
    // if (timer > 0) {
    //     entity->scale = 5.0;
    //     *entity->position = newVec3(5.0, -4.5, 0.0);
    //     sprintf(msg, "%d", timer);
    //     font_buffer_push(renderer->font, msg);
    //     for(int i=1; i<10; i++) {
    //         entity = &renderer->entities[i];
    //         if (entity->active) {
    //             entity->active = 0;
    //             entity->color = newVec3(0.0, 0.0, 0.0);
    //         }
    //     }
    // }
    // else if (timer >= -1) {
    //     entity->scale = 5.0;
    //     *entity->position = newVec3(4.95, -4.5, 0.0);
    //     font_buffer_push_color(
    //         renderer->font, "GO!", newVec3(1.0, 1.0, 0.0)
    //     );
    // }
    // else if (game_ctx->game_over) {
    //     entity->scale = 5.0;
    //     *entity->position = newVec3(4.5, -4.5, 0.0);
    //     font_buffer_push_color(
    //         renderer->font, "GAME OVER", newVec3(1.0, 0.0, 0.0)
    //     );
    //     font_buffer_push_color(
    //         renderer->font, game_ctx->msg, newVec3(1.0, 0.0, 0.0)
    //     );
    //     font_buffer_push_color(
    //         renderer->font, "R to Restart", newVec3(1.0, 1.0, 0.0)
    //     );
    //     font_buffer_push_color(
    //         renderer->font, game_ctx->msg, newVec3(1.0, 0.0, 0.0)
    //     );
    //     random_experiment = 0;
    //     stage = -1;
    //     if (game_ctx->max_points < game_ctx->points) {
    //         game_ctx->max_points = game_ctx->points;
    //     }
    //     game_ctx->points = 0;
    //     for(int i=1; i<10; i++) {
    //         entity = &renderer->entities[i];
    //         if (entity->active) {
    //             entity->color = newVec3(1.0, 0.0, 0.0);
    //         }
    //     }
    // }
    // else {
    //     *entity->position = newVec3(0.0, 0.0, 0.0);
    //     entity->scale = 1.0;

    //     sprintf(msg, "TIME: %.3f", time - game_ctx->start_time);
    //     font_buffer_push(renderer->font, msg);
    //     sprintf(
    //         msg, "FPS: %.3f | %.3f ms", 1.0/second_per_frame, second_per_frame
    //     );
    //     font_buffer_push(renderer->font, msg);
    //     sprintf(msg, "POINTS: %d", game_ctx->points);
    //     font_buffer_push_color(renderer->font, msg, newVec3(1.0, 1.0, 0.0));
    //     sprintf(msg, "MAX POINTS: %d", game_ctx->max_points);
    //     font_buffer_push_color(renderer->font, msg, newVec3(1.0, 1.0, 0.0));
    // }


    // if (show_debug_info) {
    //     handle_debug_info(ctx, renderer, camera, second_per_frame);
    // }
    
}

