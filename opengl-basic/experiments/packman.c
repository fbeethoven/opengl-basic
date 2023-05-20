#define GLAD_GL_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "packman.h"
#include "../animation.h"
#include "../mesh.h"
#include "../font.h"
#include "../utils/file_handler.h"
#include "../utils/helpers.h"


float player_rotation;
int random_experiment;
int pulse_r;
int stage;


int animation_test;
int do_animation_toggle;
int show_skeleton;
int show_skeleton_toggle;
int anim_play;
int anim_play_toggle;


int game_run() {
    animation_test = 0;
    do_animation_toggle = 0;
    show_skeleton = SHOW_SKELETON;
    show_skeleton_toggle = 0;
    anim_play = ANIMATION_PLAY;
    anim_play_toggle = 0;
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
    sync_entities(game_ctx, &renderer);

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


    ArrayList *joints = renderer.animation_controller->joints;
    Entity *entity_skeleton;
    for(int i=0; i<joints->counter; i++) {
        entity_skeleton = &renderer.debug_entities[i];
        entity_skeleton->active = SHOW_SKELETON;
    }
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
    double second_per_frame = time - ctx->previous_time;
    ctx->previous_time = time;
    game_ctx->current_time = time;


    Entity *entity;


#if 0  // Animation Experiment
    if (time >= 5.0 && animation_test == 0) {
        Vec3 translation = newVec3(0.0, 1.0, 0.0);
        Vec4 rotation = newVec4(sin(1.57), 0.0, 0.0, cos(1.57));
        Vec3 scale = newVec3(1.0, 1.0, 1.0);

        Mat4 C = Mat4I();
        C = mat4_translate(&translation, &C);
        Quat q = quat_new(rotation.x, rotation.y, rotation.z, rotation.w);
        q = quat_normalize(q);
        Mat4 R = quat_to_mat4(q);
        C = mat4_multiply(&C, &R);
        mat4_scale(&scale, &C);
        C = mat4_transpose(&C);

        // C = Mat4I();

        animation_test = 1;
        ArrayList *joints = renderer->animation_controller->joints;
        for(int i=0; i<joints->counter; i++) {
            entity = &renderer->debug_entities[i];
            Vec4 result = newVec4(
                entity->position->x, entity->position->y, entity->position->z, 1.0
            );
            result = vec4_multiply(&C, &result);
            entity->position->x = result.x;
            entity->position->y = result.y;
            entity->position->z = result.z;
        }
    }
#endif

#if 1  // Animation Experiment
    
    float time_warp = 1.0;

    AnimationController *anim = renderer->animation_controller;
    ArrayList *joints = renderer->animation_controller->joints;
    for(int i=0; i<joints->counter; i++) {
        Vec4 origin = newVec4(0.0, 0.0, 0.0, 1.0);
        Joint joint = arr_get(joints, Joint, i);
        Mat4 C = joint.inverse_bind_matrix;
        C = mat4_inverse(&C);
        C = mat4_transpose(&C);

        Vec4 result = vec4_multiply(&C, &origin);
        entity = &renderer->debug_entities[i];
        if (!entity->active) {
            break;
        }
        entity->position->x = result.x;
        entity->position->y = result.y;
        entity->position->z = result.z;
    }


    if (anim_play) {
        if ((anim->current_time + time_warp * second_per_frame) < 3.833) {
            animation_update(anim, time_warp * second_per_frame);
        }
        else {
            anim->current_time = 0.0;
            animation_update(anim, 0.0);
        }
    } else {
            animation_update(anim, 0.0);
    }

    for(int i=0; i<joints->counter; i++) {
        entity = &renderer->debug_entities[i];
        if (!entity->active) {
            break;
        }
        Joint joint = arr_get(joints, Joint, i);
        Mat4 D = joint.local_transform;

        // D = mat4_inverse(&D);
        D = mat4_transpose(&D);
        Vec4 result = newVec4(
            entity->position->x, entity->position->y, entity->position->z, 1.0
        );
        result = vec4_multiply(&D, &result);
        entity->position->x = result.x;
        entity->position->y = result.y;
        entity->position->z = result.z;
    }

#endif  // Animation Experiment








    entity = get_entity_selected(renderer);
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
        float button_x_pos = 50.0;
        float button_y_pos = 50.0;
        float button_step = 25.0;

        entity = &renderer->debug_entities[99];
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

        button_text = (
            renderer->do_animation ? 
            "Animation Shader: ON" : "Animation Shader: OFF"
        );
        button_y_pos += button_step;
        if (ui_button(
            ctx, renderer, newVec2(button_x_pos, button_y_pos), button_text)
        ) {
            if (do_animation_toggle == 0) {
                do_animation_toggle = 1;
            }
        }
        else {
            if (do_animation_toggle == 1) {
                renderer->do_animation = 1 - renderer->do_animation;
                do_animation_toggle = 0;
            }
        }
        button_y_pos += button_step;
        button_text = show_skeleton ? "Skeleton: ON" : "Skeleton: OFF";
        if (ui_button(
            ctx, renderer, newVec2(button_x_pos, button_y_pos), button_text)
        ) {
            if (show_skeleton_toggle == 0) {
                show_skeleton_toggle = 1;
            }
        }
        else {
            if (show_skeleton_toggle == 1) {
                show_skeleton_toggle = 0;
                show_skeleton = 1 - show_skeleton;

                int total_joints = (
                    renderer->animation_controller->joints->counter
                );

                for(int i=0; i<total_joints; i++) {
                    entity = &renderer->debug_entities[i];
                    entity->active = show_skeleton;
                }
            }
        }
        button_y_pos += button_step;
        button_text = anim_play ? "Animation: Play" : "Animation: Pause";
        if (ui_button(
            ctx, renderer, newVec2(button_x_pos, button_y_pos), button_text)
        ) {
            if (anim_play_toggle == 0) {
                anim_play_toggle = 1;
            }
        }
        else {
            if (anim_play_toggle == 1) {
                anim_play_toggle = 0;
                anim_play = 1 - anim_play;
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


    // if (show_debug_info) {
    //     handle_debug_info(ctx, renderer, camera, second_per_frame);
    // }
    
}
