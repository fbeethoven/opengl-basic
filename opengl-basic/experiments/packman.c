#define GLAD_GL_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION


#include "packman.h"
#include "../mesh.h"
#include "../font.h"
#include "../utils/file_handler.h"
#include "../utils/helpers.h"
#include "../memory.h"

#include "../audio.h"
int play_audio;
double last_play;


float player_rotation;
int random_experiment;
int pulse_r;
int stage;


// Collision Test
int mouse_press;
int selection;
int selection_press;


UIManager *ui_manager;



int game_run() {
    // TODO(CLEAN UP):
    // [ ] Improve Font rendereing
    // [ ] Input System
    // [ ] GUI Editor for entities
    //
    // -----------------------------
    // [ ] Implement New Experiment
    // -----------------------------
    //
    // [ ] Add Layers and scenes.
    //      Each Scene will have:
    //          - a hierarchy of entities.
    //          - a stack of layers.
    //      Each layer will have:
    //          - a list of components.
    //          - a callback on_render_layer(Renderer, Layer).
    //      The main loop will call render(*renderer, *scene) and this
    //      function will be:
    //          scene->on_render_begin(renderer);
    //          FOR_ALL_PTR(layer, scene->layers) {
    //              layer->on_render_layer(renderer, layer);
    //          }
    //          scene->on_render_end(renderer);
    // [X] Lists with types
    // [X] Clean warnings
    // [ ] Remove experiment helper
    //      [ ] add utils/random module
    //      [ ] move entity helpers and load assets to graphics module
    //      [X] move animation params to animation module
    //          (including debug options)
    // [X] Fix Entity Struct
    //      [ ] Entities will have a list of components:
    //          - Transform component (it will be a tree)
    //          - Mesh component (static vs dynamic)
    //          - Mesh collider (aabb vs spherical vs octo-tree)
    //              - Can we calculate the octo-tree on loading the mesh?
    //          - Physic Component (for movement)
    //          - Can we make the camera a component? We could look through
    //              the eyes of any entity.
    // [ ] Fix Render Struct
    //      [X] Use list for entities first.
    //          In the future use a layer stack with callbacks
    //      [ ] Use list for models. Maybe hashmap?
    //      [ ] Have renderer to render a scene.
    // [ ] Clean up memory at the end of the program.
    // [X] Improve UI
    // [ ] Event System (Maybe we just want a toggle?)
    // [ ] Remove all debug printing


    // TODO(Weekend):
    // [X] Add sound (not really part of clean up but I 

    // TODO(OLD):
    //  [X] add color to textures (gamma correction)
    //  [X] gui buttons
    //  [ ] gui sliders
    //  [X] collision:
    //      [X] AABB
    //      [X] Spherical
    //  [X] add gravity
    //  [ ] rotate entity towards target point
    //  [X] camera movement using mouse
    //  [ ] quad sprite facing the camera
    //  [X] mouse picker
    //      [X] Fix issue with camera rotations
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
    
    // Collision Test
    mouse_press = 0;
    selection = 1;
    selection_press = 0;

    stage = -1;
    distance_from_player = 5.0;
    random_experiment = 0;

    entity_index = 0;

    GraphicsContext ctx;
    if(graphics_init(&ctx) != 0) {
        return -1;
    }

    sound_init();

    Renderer renderer = {0};
    init_render_handler(&ctx, &renderer);

    ui_manager = ui_init(&ctx, &renderer);

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
    GraphicsContext *ctx, Renderer *renderer, Camera *camera
) {
    char msg[500];
    sprintf(msg, "FPS: %.3f | %.3f ms", 1.0/ctx->dtime, ctx->dtime);
    font_buffer_push(renderer->font, msg);

    sprintf(msg, "Current Time: %f", ctx->current_time);
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

    free_rts_camera_movement(ctx, &camera_params);
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
        ctx->resolution_has_changed = 1;
    }
    else {
        ctx->resolution_has_changed = 0;
    }
    if (ctx->resolution_has_changed || renderer->FOV != FOV) {
        renderer->FOV = FOV;
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
    double second_per_frame = time - ctx->current_time;
    ctx->dtime = second_per_frame;
    ctx->current_time = time;
    game_ctx->current_time = time;


    Entity *entity;

    // mouse picking
    entity = LIST_GET_PTR(renderer->entities, 1);
    entity->model = &game_ctx->models[selection];
    entity->position = mouse_to_plane(
        ctx, renderer, camera, newVec3(0.0, 1.0, 0.0), 0.0
    );

    Vec3 mouse_dir = mouse_to_world(ctx, renderer, camera);
#if BOX_COLLITION
    Vec3 box_min, box_max;
#endif
    for (int i=2; i<renderer->entities->counter; i++) {
        entity = LIST_GET_PTR(renderer->entities, i);

#if BOX_COLLITION
        box_min = newVec3(
            entity->position.x - 0.5,
            entity->position.y - 0.5,
            entity->position.z - 0.5
        );
        box_max = newVec3(
            entity->position.x + 0.5,
            entity->position.y + 0.5,
            entity->position.z + 0.5
        );
        if (ray_to_aabb(camera->position, mouse_dir, box_min, box_max)) {
#else 
        if (
            ray_to_sphere(camera->position, mouse_dir, entity->position, 1.0)
        ) {
#endif
            entity->color = newVec3(1.0, 0.0, 0.0);
        }
        else {
            entity->color = newVec3(0.0, 0.0, 0.0);
        }
    }

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
    }
    if (toggle_button_press(ctx, GLFW_KEY_R, &pulse_r)){
        game_ctx->start_time = time;
        camera->position = newVec3(0.0, 60.0, 0.0);
        game_ctx->game_over = 0;
        player_is_grounded = 0;
        camera->pitch = 2.2;
        camera->yaw = 1.57;
    }

    camera_focus_movement(ctx, camera, second_per_frame, distance_from_player);

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

    
    if (glfwGetMouseButton(ctx->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (mouse_press == 0) {
            mouse_press = 1;
            entity = list_push(renderer->entities, Entity);
            entity->model = &game_ctx->models[selection];
            entity->model_name = selection;
            float scale = 1.0;
            entity->scale = newVec3(scale, scale, scale);
            entity->active = 1;
            entity->position =  mouse_to_plane(
                ctx, renderer, camera, newVec3(0.0, 1.0, 0.0), 0.0
            );

        }
    }
    else if (mouse_press == 1) {
        mouse_press = 0;
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_N) == GLFW_PRESS) {
        if (selection_press == 0) {
            selection_press = 1;
            selection += 1;
            if (selection >= ModelType_Count) {
                selection = 1;
            }
        }
    } else {
        selection_press = 0;
    }

    ui_reset(ui_manager);
    ui_test_button(ui_manager, renderer);
}

