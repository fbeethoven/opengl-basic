#define GLAD_GL_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "packman.h"
#include "../mesh.h"
#include "../font.h"
#include "../utils/file_handler.h"
#include "../utils/helpers.h"


float player_rotation;
int random_experiment;
int pulse_r;
int stage;

int game_run() {
    // TODO:
    //  [X] add color to textures (gamma correction)
    //  [ ] gui buttons
    //  [ ] gui sliders
    //  [ ] collision:
    //      [ ] AABB
    //      [ ] SAT
    //      [ ] Spherical
    //  [ ] add gravity
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
    //  [ ] Fix file_handler for big endian

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
    sync_entities(game_ctx, &renderer);

    Camera camera = {0};

    // camera.position = newVec3(0.0, 60.0, 0.0);
    // camera.centre = newVec3(0.0, 55.0, 0.0);
    // camera.pitch = 2.2;
    // camera.yaw = 1.57;
    camera.position = newVec3(0.0, 1.0, 5.0);
    camera.centre = newVec3(0.0, 1.0, 0.0);
    camera.pitch = 1.57;
    camera.yaw = -1.57;

    Light light = {0};
    light.position = newVec3(0.0, -1.0, 0.0);
    light.color = newVec3(0.8, 0.8, 1.0);

    renderer.light = &light;

    load_assets(&ctx, &renderer, game_ctx, &font, &camera);


    // Entity *entity = &renderer.gui_entities[1];
    // gui_quad_in_pos(
    //     &ctx,  entity, newVec2(0.5*ctx.width, 0.5*ctx.height),
    //     newVec2(100.0, 250.0), newVec3(1.0, 1.0, 1.0)
    // );


    while (!glfwWindowShouldClose(ctx.window)) {
        handle_input(&ctx, &renderer, &camera);

        render(&renderer, &camera);

        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


Entity *get_entity_selected(Renderer *renderer) {
    return &renderer->debug_entities[entity_index];

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
    camera_follow_player(player->position, player->rotation_y, &camera_params);
}


void update_graphic_state(GraphicsContext *ctx, Renderer *renderer) {
    float prev_width = ctx->width;
    float prev_height = ctx->height;
    glfwGetWindowSize(ctx->window, &ctx->width, &ctx->height);
    if (prev_width != ctx->width || prev_height != ctx->height) {
        reload_projection_matrix(ctx, renderer);
    }

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
    double second_per_frame = time - ctx->previous_time;
    ctx->previous_time = time;
    game_ctx->current_time = time;

    Entity *entity = get_entity_selected(renderer);

    // mouse picking
    // entity = &renderer->entities[10];
    // Vec3 position = mouse_to_plane( ctx, renderer, camera, newVec3(0.0, 1.0, 0.0), 0.0);
    // *entity->position = mouse_to_plane( ctx, renderer, camera, newVec3(0.0, 1.0, 0.0), 0.0);



    if ( (time - game_ctx->start_time) > 5.0 && stage < 0) {
        for (int i=1; i<10; i++) {
            entity = &renderer->entities[i];
            entity->active = 0;
        }
        random_experiment = 1;
        stage = 0;
    }
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

    entity = get_entity_selected(renderer);

    if(
        glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_Q) == GLFW_PRESS
    ) {
        glfwSetWindowShouldClose(ctx->window, GL_TRUE);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_X) == GLFW_PRESS) {
        camera_reset(camera);
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
        entity = get_entity_selected(renderer);
        entity->color = newVec3(0.0, 0.0, 0.0);

        entity_index++;
        entity = get_entity_selected(renderer);
        if (entity->active == 0) {
            entity_index = 0;
            entity = get_entity_selected(renderer);
        }
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
    char msg[500];
    if (show_debug_info) {
        if (ui_button(ctx, renderer, newVec2(50.0, 50.0), "Show Mesh Wire")) {
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
    }
    else {
        entity = &renderer->gui_entities[0];
        entity->active = 0;
    }
    entity = get_entity_selected(renderer);

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


    Joint *root = renderer->root;


    if (glfwGetKey(ctx->window, GLFW_KEY_H) == GLFW_PRESS) {
        root->local_transform.translation.x -= 0.1;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_L) == GLFW_PRESS) {
        root->local_transform.translation.x += 0.1;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_J) == GLFW_PRESS) {
        root->local_transform.translation.y -= 0.1;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_K) == GLFW_PRESS) {
        root->local_transform.translation.y += 0.1;
    }

    joint_update_all(root);


    // if (show_debug_info) {
    //     handle_debug_info(ctx, renderer, camera, second_per_frame);
    // }
}
