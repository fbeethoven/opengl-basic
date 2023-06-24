#include "helpers.h"
#include "file_handler.h"


void quad_in_pos(
    GraphicsContext *ctx, Entity *entity, Vec3 center, Vec2 size, Vec3 color
) {
    Mesh quad_mesh = {0};
    Vec3 vert[4];
    quad_mesh.vertices = vert;
    Vec2 uvs_coords[4];
    quad_mesh.uvs = uvs_coords;
    Vec3 normal[4];
    quad_mesh.normal = normal;
    Vec3 color_buffer[4];
    quad_mesh.color = color_buffer;
    unsigned int ind[10];
    quad_mesh.indices = ind;

    draw_quad(&quad_mesh, newVec3(0, 0, 0.0), color, size);
    BaseModel *quad_model = (BaseModel *)malloc(sizeof(BaseModel));
    load_data_to_model(
        quad_model, (float *)quad_mesh.vertices, quad_mesh.indices,
        3 * quad_mesh.vertices_len * sizeof(float),
        quad_mesh.indices_len * sizeof(unsigned int)
    );
    load_empty_texture_to_model(
        quad_model, (float *)quad_mesh.uvs, 
        2 * quad_mesh.uvs_len * sizeof(float)
    );
    glBindVertexArray(quad_model->vao);
    store_float_in_attributes(
        &quad_model->color,
        2,
        3,
        3 * quad_mesh.color_len * sizeof(float),
        (float *)quad_mesh.color
    );
    log_if_err("Issue loading quad color\n");
    quad_model->vertex_count = quad_mesh.indices_len;

    entity->model = quad_model;

    entity->position = center;
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);
}


void gui_quad_in_pos(
    GraphicsContext *ctx, Entity *entity, Vec2 center, Vec2 size, Vec3 color
) {
    Mesh quad_mesh = {0};
    Vec3 vert[4];
    quad_mesh.vertices = vert;
    Vec2 uvs_coords[4];
    quad_mesh.uvs = uvs_coords;
    Vec3 normal[4];
    quad_mesh.normal = normal;
    Vec3 color_buffer[4];
    quad_mesh.color = color_buffer;
    unsigned int ind[10];
    quad_mesh.indices = ind;


    Vec2 new_size = newVec2(
        2 * size.x/(float)ctx->width,
        2 * size.y/(float)ctx->height
    );
    draw_quad(&quad_mesh, newVec3(0.0, 0.0, 0.0), color, new_size);
    BaseModel *quad_model = (BaseModel *)malloc(sizeof(BaseModel));
    load_data_to_model(
        quad_model, (float *)quad_mesh.vertices, quad_mesh.indices,
        3 * quad_mesh.vertices_len * sizeof(float),
        quad_mesh.indices_len * sizeof(unsigned int)
    );
    load_empty_texture_to_model(
        quad_model, (float *)quad_mesh.uvs, 
        2 * quad_mesh.uvs_len * sizeof(float)
    );
    glBindVertexArray(quad_model->vao);
    store_float_in_attributes(
        &quad_model->color,
        2,
        3,
        3 * quad_mesh.color_len * sizeof(float),
        (float *)quad_mesh.color
    );
    log_if_err("Issue loading quad color\n");
    quad_model->vertex_count = quad_mesh.indices_len;

    entity->model = quad_model;

    float pos_x = (2 * center.x/(float)ctx->width) - 1.0;
    float pos_y = 1.0 - (2 * center.y/(float)ctx->height);
    entity->position = newVec3(pos_x, pos_y, 0);
    entity->active = 1;
    entity->scale = newVec3(1.0, 1.0, 1.0);
}


void gui_quad_free(Entity *entity) {
    free(entity->model);
}


void free_fps_camera_movement(
    GraphicsContext *ctx, CameraMovementParams *params
) {
    // float player_is_grounded = params->player_is_grounded;
    float speed = params->camera_speed * params->dt * 50;
    float player_momentum = params->player_rotation;


    
    Camera *camera = params->camera;
    Vec3 foward = newVec3(
        camera->centre.x - camera->position.x,
        camera->centre.y - camera->position.y,
        camera->centre.z - camera->position.z
    );
    vec3_normalize(&foward);
    Vec3 right = vec3_cross(foward, newVec3(0.0, 1.0, 0.0));
    vec3_normalize(&right);

    if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->position.x -= speed * right.x;
        camera->position.z -= speed * right.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->position.x += speed * right.x;
        camera->position.z += speed * right.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->position.x += speed * foward.x;
        camera->position.z += speed * foward.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->position.x -= speed * foward.x;
        camera->position.z -= speed * foward.z;
    }

    int shift_press = shift_is_pressed(ctx);
    if (shift_press){
        if (glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera->position.y -= speed;
        }
    }
    else {
        if (glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera->position.y += speed;
        }
    }
    // if (
    //     player_is_grounded &&
    //     glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS
    // ) {
    //         player_momentum = -0.4;
    //         params->player_is_grounded = 0;
    // }

    camera->yaw += 0.0005 * (float)ctx->dmouse[0];
    camera->pitch += 0.0005 * (float)ctx->dmouse[1];
    ctx->dmouse[0] = 0.0;
    ctx->dmouse[1] = 0.0;

    // float gravity = 1.0;
    // if (!player_is_grounded) {
    //     player_momentum += gravity * params->dt;
    //     if (player_momentum > 3) {
    //         player_momentum = 3;
    //     }
    // }
    camera->position.y -= player_momentum;

    camera->centre.x = (
        camera->position.x + sin(camera->pitch) * cos(camera->yaw)
    );
    camera->centre.y = camera->position.y + cos(camera->pitch);
    camera->centre.z = (
        camera->position.z + sin(camera->pitch) * sin(camera->yaw)
    );

    if (
        (camera->position.x <= -100) || (camera->position.z <= -100) ||
        (camera->position.x >= 95) || (camera->position.z >= 95)
    ) {
        params->player_is_grounded = 0;
    }
    else if (camera->position.y <= 2.0) {
            camera->position.y = 2.0;
            params->player_is_grounded = 1;
            player_momentum = 0.0;
    }
    params->player_rotation = player_momentum;
}

void free_rts_camera_movement(
    GraphicsContext *ctx, CameraMovementParams *params
) {
    float speed = params->camera_speed * params->dt * 50;
    
    Camera *camera = params->camera;
    Vec3 foward = newVec3(
        camera->centre.x - camera->position.x,
        camera->centre.y - camera->position.y,
        camera->centre.z - camera->position.z
    );
    vec3_normalize(&foward);
    Vec3 right = vec3_cross(foward, newVec3(0.0, 1.0, 0.0));
    vec3_normalize(&right);

    if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->position.x -= speed * right.x;
        camera->position.z -= speed * right.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->position.x += speed * right.x;
        camera->position.z += speed * right.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->position.x += speed * foward.x;
        camera->position.z += speed * foward.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->position.x -= speed * foward.x;
        camera->position.z -= speed * foward.z;
    }

    if (shift_is_pressed(ctx)){
        if (glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera->position.y -= speed;
        }
    }
    else {
        if (glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera->position.y += speed;
        }
    }

    if (control_is_pressed(ctx)) {
        glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera->yaw += 0.0005 * (float)ctx->dmouse[0];
        camera->pitch += 0.0005 * (float)ctx->dmouse[1];
        ctx->dmouse[0] = 0.0;
        ctx->dmouse[1] = 0.0;

        editor_enter_mode(params->editor_state, EditorMode_Rotate);

    }
    else {
        glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        editor_exit_mode(params->editor_state, EditorMode_Rotate);
        // editor_enter_mode(params->editor_state, EditorMode_None);
    }

    camera->centre.x = (
        camera->position.x + sin(camera->pitch) * cos(camera->yaw)
    );
    camera->centre.y = camera->position.y + cos(camera->pitch);
    camera->centre.z = (
        camera->position.z + sin(camera->pitch) * sin(camera->yaw)
    );
}


void player_movement(GraphicsContext *ctx, PlayerMovementParams *params) {
    if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        params->player->rotation.x = params->player->rotation.x;
        params->player->rotation.y = (
            params->player->rotation.y + params->rotation_factor);
        params->player->rotation.z = params->player->rotation.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        params->player->rotation.x = params->player->rotation.x;
        params->player->rotation.y = (
            params->player->rotation.y - params->rotation_factor);
        params->player->rotation.z = params->player->rotation.z;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        params->d_player_move += params->speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        params->d_player_move -= params->speed;
    }

    params->player->position.x += (
        params->d_player_move * sinf(params->player->rotation.y)
    );
    params->player->position.z += (
        params->d_player_move * cosf(params->player->rotation.y)
    );
}


void camera_follow_player(
    Vec3 *pos, float rot_y, CameraMovementParams *params
) {
    float horizontal_distance = (
        params->distance_from_player * cosf(params->camera->pitch)
    );
    float vertical_distance = (
        params->distance_from_player * sinf(params->camera->pitch)
    );
    float theta = params->camera->yaw + rot_y;

    float target_x = pos->x - horizontal_distance*sinf(theta);
    float target_z = pos->z - horizontal_distance*cos(theta);
    float target_y = pos->y + vertical_distance;

    Vec3 new_camera_pos = newVec3(target_x, target_y, target_z);

    float distance = vec3_distance(&params->camera->position, &new_camera_pos);
    if (params->camera_speed < distance) {
        float t = params->camera_speed/distance;
        params->camera->position = vec3_lerp(
            params->camera->position, new_camera_pos, t
        );
    }
    else {
        params->camera->position = new_camera_pos;
    }
    params->camera->centre = *pos;
}


void camera_movement(GraphicsContext *ctx, CameraMovementParams *params) {
    Camera *camera = params->camera;
    if (glfwGetKey(ctx->window, GLFW_KEY_H) == GLFW_PRESS) {
        camera->yaw -= params->speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_L) == GLFW_PRESS) {
        camera->yaw += params->speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_J) == GLFW_PRESS) {
        camera->pitch -= params->speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_K) == GLFW_PRESS) {
        camera->pitch += params->speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {

        params->distance_from_player -= params->speed;  
        if (params->distance_from_player <= 0.5) {
            params->distance_from_player = 0.5;
        }
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        params->distance_from_player += params->speed;
    }
}


int toggle_button_press(
    GraphicsContext *ctx, unsigned int key, int *is_press_bool
) {
    if (glfwGetKey(ctx->window, key ) == GLFW_PRESS){
        if (*is_press_bool == 0) {
            *is_press_bool = 1;
            return 1;
        }
    } else {
        *is_press_bool = 0;
    }
    return 0;
}

int control_is_pressed(GraphicsContext *ctx) {
    if (
        glfwGetKey(ctx->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS
    ) {
        return 1;
    }
    return 0;
}

int shift_is_pressed(GraphicsContext *ctx) {
    if (
        glfwGetKey(ctx->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(ctx->window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
    ) {
        return 1;
    }
    return 0;
}


void draw_quad_in_pixels(
    GraphicsContext *ctx, Mesh *mesh, Vec2 center, Vec3 color, Vec2 side
) {
    float x = 2 * (center.x/ctx->width) - 1;
    float y = 1 - 2 * (center.y/ctx->height);

    draw_quad(
        mesh, newVec3(x, y, 0), color,
        newVec2(side.x/ctx->width, side.y/ctx->height)
    );
}


void camera_reset(Camera *camera) {
    camera->pitch = 2.2;
    camera->yaw = 1.57;
}

RayToPlaneHit ray_to_plane_from(Vec3 origin, Vec3 toward, Vec3 normal, float distance) {
    Vec3 dir = newVec3(
        toward.x - origin.x, toward.y - origin.y, toward.z - origin.z
    );
    return ray_to_plane(origin, dir, normal, distance);
}

RayToPlaneHit ray_to_plane(Vec3 origin, Vec3 dir, Vec3 normal, float distance) {
    Vec3 normalize_dir = dir;
    vec3_normalize(&normalize_dir);
    RayToPlaneHit result = {0};

    float t = vec3_dot(&normalize_dir, &normal);
    if (t >= 0) { 
        return result;
    }
    t = (distance - vec3_dot(&origin, &normal)) / t;

    normalize_dir.x *= t;
    normalize_dir.y *= t;
    normalize_dir.z *= t;
    result.hit = vec3_add(&origin, &normalize_dir);
    result.is_hit = 1;
    return result;
}

int ray_to_sphere(Vec3 origin, Vec3 dir, Vec3 center, float radius) {
    Vec3 dist_vec = newVec3(
        origin.x - center.x, origin.y - center.y, origin.z - center.z
    ); 
    float b = vec3_dot(&dist_vec, &dir); 
    float c = vec3_dot(&dist_vec, &dist_vec) - (radius*radius); 

    if (c > 0.0f && b > 0.0f) {
        return 0;
    }

    float discr = b*b - c; 
    if (discr < 0.0f) {
        return 0; 
    }
    // distance to sphere ==>  t = -b - sqrt(discr); 
    // intersection poitn ==>  origin + t * dir; 
    return 1;
}

int ray_to_aabb(Vec3 origin, Vec3 dir, Vec3 box_min, Vec3 box_max) {
    Vec3 dir_inv = newVec3(
        dir.x == 0.0 ? 100.0: (1.0/dir.x),
        dir.y == 0.0 ? 100.0: (1.0/dir.y),
        dir.z == 0.0 ? 100.0: (1.0/dir.z)
    );
    Vec3 t_min = newVec3(
        (box_min.x - origin.x) * dir_inv.x,
        (box_min.y - origin.y) * dir_inv.y,
        (box_min.z - origin.z) * dir_inv.z
     );
    Vec3 t_max = newVec3(
        (box_max.x - origin.x) * dir_inv.x,
        (box_max.y - origin.y) * dir_inv.y,
        (box_max.z - origin.z) * dir_inv.z
     );
    Vec3 t1 = newVec3(
        MIN(t_min.x, t_max.x), MIN(t_min.y, t_max.y), MIN(t_min.z, t_max.z)
    );
    Vec3 t2 = newVec3(
        MAX(t_min.x, t_max.x), MAX(t_min.y, t_max.y), MAX(t_min.z, t_max.z)
    );
    float t_near = MAX(MAX(t1.x, t1.y), t1.z);
    float t_far = MIN(MIN(t2.x, t2.y), t2.z);
    
    if (t_far < 0 || (t_near > t_far)) {
        // distance is t_far
        return 0;
    }
    // distance is t_near 
    return 1;
}


Vec3 screen_to_world(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera, Vec2 position
) {
    float x = 2 * position.x/ctx->width - 1.0;
    float y = 1.0 - 2 * position.y/ctx->height;

    Vec4 rel_pos = newVec4(x, y, -1.0, 1.0);

    Mat4 projection_inverse = mat4_inverse(&renderer->projection_matrix);


    Mat4 view_matrix = mat4_look_at(
        camera->position, 
        camera->centre,
        newVec3(0.0, 1.0, 0.0)
    );

    rel_pos = vec4_multiply(&projection_inverse, &rel_pos);
    rel_pos.z = -1.0;
    rel_pos.w = 0.0;
    rel_pos = vec4_multiply(&view_matrix, &rel_pos);

    Vec3 result = newVec3(rel_pos.x, rel_pos.y, rel_pos.z);
    vec3_normalize(&result);
    return  result;
}


Vec3 mouse_to_world(GraphicsContext *ctx, Renderer *renderer, Camera *camera) {
    float x = 2 * (float)ctx->mouse_position[0]/ctx->width - 1.0;
    float y = 1.0 - 2 * (float)ctx->mouse_position[1]/ctx->height;

    Vec4 rel_pos = newVec4(x, y, -1.0, 1.0);

    Mat4 projection_inverse = mat4_inverse(&renderer->projection_matrix);


    Mat4 view_matrix = mat4_look_at(
        camera->position, 
        camera->centre,
        newVec3(0.0, 1.0, 0.0)
    );

    rel_pos = vec4_multiply(&projection_inverse, &rel_pos);
    rel_pos.z = -1.0;
    rel_pos.w = 0.0;
    rel_pos = vec4_multiply(&view_matrix, &rel_pos);

    Vec3 result = newVec3(rel_pos.x, rel_pos.y, rel_pos.z);
    vec3_normalize(&result);
    return  result;
}

RayToPlaneHit mouse_to_plane(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera,
    Vec3 normal, float distance
) {
    Vec3 dir = mouse_to_world(ctx, renderer, camera);
    return ray_to_plane(camera->position, dir, normal, distance);
}


int ui_button(
    GraphicsContext *ctx, Renderer *renderer, Vec2 position, char *text
) {
    Vec3 font_color = newVec3(0.88, 0.88, 0.88);
    Vec2 padding = newVec2(10.0, 5.0);

    Vec2 button_position = _font_buffer_push(
        renderer->font, text, position, font_color
    );

    float x_min = position.x - padding.x; 
    float x_max = x_min + button_position.x + 2*padding.x;
    button_position.x = x_max - x_min;
    float y_min = position.y - button_position.y - padding.y;
    float y_max = y_min + button_position.y + 2*padding.y;
    button_position.y = y_max - y_min;
    int is_hot = 0;
    if (
        (x_min <= ctx->mouse_position[0]) &&
        (x_max >= ctx->mouse_position[0]) &&
        (y_min <= ctx->mouse_position[1]) &&
        (y_max >= ctx->mouse_position[1])
    ) {
        font_color = newVec3(0.88, 0.88, 0.5);
        is_hot = 1;
    }
    _font_buffer_push(renderer->font, text, position, font_color);

    // Entity *entity = &renderer->gui_entities[0];
    // gui_quad_in_pos(
    //     ctx,  entity, newVec2(x_min, y_min), button_position, button_color
    // );

    int result = 0;
    if (is_hot &&
        glfwGetMouseButton(ctx->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS
    ) {
        result = 1;
    }

    return result;
}


void ui_color_picker(GraphicsContext *ctx, Entity *entity) {
    Vec3 vertices[] = {
        newVec3(-1.0, 1.0, 0.0),
        newVec3(-1.0, 0.0, 0.0),
        newVec3(0.0, 0.0, 0.0),
        newVec3(0.0, 1.0, 0.0),

        newVec3(-0.8, 0.8, 0.0),
        newVec3(-0.8, 0.7, 0.0),
        newVec3(-0.5, 0.7, 0.0),
        newVec3(-0.5, 0.8, 0.0),

        newVec3(-0.8, 0.6, 0.0),
        newVec3(-0.8, 0.5, 0.0),
        newVec3(-0.5, 0.5, 0.0),
        newVec3(-0.5, 0.6, 0.0)
    };

    Vec2 uvs[] = {
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),

        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),

        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0),
        newVec2(0.0, 0.0)
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 0, 3,
        4, 5, 6,
        6, 4, 7,
        8, 9, 10,
        10, 8, 11
    };

    Vec4 color[] = {
        newVec4(0.3, 0.3, 0.3, 1.0),
        newVec4(0.3, 0.3, 0.3, 1.0),
        newVec4(0.3, 0.3, 0.3, 1.0),
        newVec4(0.3, 0.3, 0.3, 1.0),

        newVec4(0.0, 0.0, 0.0, 1.0),
        newVec4(0.0, 0.0, 0.0, 0.0),
        newVec4(0.0, 0.0, 0.0, 0.0),
        newVec4(0.0, 0.0, 0.0, 1.0),

        newVec4(1.0, 1.0, 1.0, 1.0),
        newVec4(1.0, 1.0, 1.0, 0.0),
        newVec4(1.0, 1.0, 1.0, 0.0),
        newVec4(1.0, 1.0, 1.0, 1.0)
    };


    load_data_to_model(entity->model, (float *)vertices, indices,
        sizeof(vertices), sizeof(indices)
    );
    entity->model->vertex_count = sizeof(indices)/sizeof(indices[0]);

    load_empty_texture_to_model(entity->model, (float *)uvs, sizeof(uvs));
    glBindVertexArray(entity->model->vao);
    store_float_in_attributes(
        &entity->model->color, 2, 4, sizeof(color), (float *) color
    );
    log_if_err("Issue loading quad color\n");

}



UIWidget *ui_get_widget(UIManager *ui_manager) {
    UIWidget *result;
    if (ui_manager->free_widget) {
        result = ui_manager->free_widget;
        ui_manager->free_widget = result->next;
        memset(result, 0, sizeof(UIWidget));
    }
    else {
        result = calloc(1, sizeof(UIWidget));
    }
    return result;
}

UIManager *ui_init(GraphicsContext *ctx, Renderer *renderer) {
    UIManager *ui_manager = calloc(1, sizeof(UIManager));
    ui_manager->ctx = ctx;
    UIWidget *root = ui_get_widget(ui_manager);
    root->rect = newVec4(0.0, 0.0, (float)ctx->width, (float)ctx->height);
    root->child_position = newVec2(0.0, 0.0);
    ui_manager->root_widget = root;
    ui_manager->current_parent_widget = root;
    ui_manager->gui_entities = renderer->gui_entities;
    ui_manager->renderer = renderer;
    return ui_manager;
}

void ui_reset(UIManager *ui_manager) {
    Entity *entity;
    for (int i=0; i<ui_manager->gui_entities->counter; i++) {
        entity = LIST_GET_PTR(ui_manager->gui_entities, i);
        entity->active = 0;
    }
    if (ui_manager->ctx->resolution_has_changed) {
        ui_manager->root_widget->rect = newVec4(
            0.0, 0.0,
            (float)ui_manager->ctx->width,
            (float)ui_manager->ctx->height
        );
    }
    ui_manager->current_parent_widget = ui_manager->root_widget;
    ui_manager->root_widget->child_position = newVec2(0.0, 0.0);
    ui_manager->current_child_widget = 0;
}


Entity *ui_get_new_entity(UIManager *ui_manager) {
    BaseModel *model = malloc(sizeof(BaseModel));

    log_if_err("[MeshComponent] Issue before initialization");
    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);

    glGenBuffers(1, &model->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    U32 ibo_size = 6 * MeshCapacity * sizeof(U32);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_size, 0, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    U32 vbo_size = 3 * 4 * MeshCapacity * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vbo_size, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    log_if_err("[MeshComponent] Issue during initialization");

    glGenBuffers(1, &model->uv);
    glBindBuffer(GL_ARRAY_BUFFER, model->uv);
    U32 uv_size = 2 * 4 * MeshCapacity * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, uv_size, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glGenBuffers(1, &model->color);
    glBindBuffer(GL_ARRAY_BUFFER, model->color);
    U32 color_size = 4 * 4 * MeshCapacity * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, color_size, 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    log_if_err("[MeshComponent] Issue during initialization");

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &model->texture_id);
    glBindTexture(GL_TEXTURE_2D, model->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int texture = 0xffffffff;

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &texture
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    Entity *entity = list_push(ui_manager->gui_entities, Entity);
    entity->model = model;
    entity->scale = newVec3(1.0, 1.0, 1.0);
    return entity;
}

Entity *ui_get_entity(UIManager *ui_manager) {
    Entity *entity;
    int found = 0;
    for (int i=0; i<ui_manager->gui_entities->counter; i++) {
        entity = LIST_GET_PTR(ui_manager->gui_entities, i);
        if (!entity->active) {
            found = 1;
            break;
        }
    }
    if (!found) {
        entity = ui_get_new_entity(ui_manager);
    }
    entity->active = 1;
    entity->model->vertex_count = 0;
    return entity;
}


void ui_entity_update(
    Entity *entity, Vec4 position, Vec4 entity_color_a, Vec4 entity_color_b, 
    Vec4 entity_color_c, Vec4 entity_color_d
) {
    BaseModel *model = entity->model;
    float vertices[] = {
        position.x             , position.y             , 0.0,
        position.x             , position.y + position.w, 0.0,
        position.x + position.z, position.y + position.w, 0.0,
        position.x + position.z, position.y             , 0.0
    };

    float uvs[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    float color[] = {
        entity_color_a.x, entity_color_a.y, entity_color_a.z, entity_color_a.w,
        entity_color_b.x, entity_color_b.y, entity_color_b.z, entity_color_b.w,
        entity_color_c.x, entity_color_c.y, entity_color_c.z, entity_color_c.w,
        entity_color_d.x, entity_color_d.y, entity_color_d.z, entity_color_d.w
    };

    unsigned int indices[] = {0, 1, 2, 2, 0, 3};
    model->vertex_count = 6;


    glBindVertexArray(model->vao);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindBuffer(GL_ARRAY_BUFFER, model->uv);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uvs), uvs);

    glBindBuffer(GL_ARRAY_BUFFER, model->color);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), color);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
    log_if_err("Issue with subdata\n");

}

UIWidget *ui_push_child_(
    UIManager *ui_manager, float width, float height, int add_x, int add_y,
    int flags, Vec4 color_a, Vec4 color_b, Vec4 color_c, Vec4 color_d,
    char *text, Vec3 font_color
) {
    UIWidget *parent = ui_manager->current_parent_widget;
    UIWidget *result;
    if (!ui_manager->current_child_widget && parent->first){
        result = parent->first;
        ui_manager->current_child_widget = result;
    }
    else if (
        ui_manager->current_child_widget &&
        ui_manager->current_child_widget->next
    ) {
        result = ui_manager->current_child_widget->next; 
        ui_manager->current_child_widget = result;
    }
    else {
        result = ui_get_widget(ui_manager);
        result->parent = parent;
        if (!ui_manager->current_child_widget) {
            parent->first = result;
        }
        else {
            ui_manager->current_child_widget->next = result;
        }
        ui_manager->current_child_widget = result;
    }

    float x_size = parent->rect.z * width;
    float y_size = (flags & UIFlags_Square) ? x_size : parent->rect.w * height;
    result->rect = newVec4(
        parent->child_position.x, parent->child_position.y, x_size, y_size);
    result->child_position = newVec2(
        parent->child_position.x, parent->child_position.y);
    
    if (add_x) {
        parent->child_position.x += x_size;
    }
    if (add_y) {
        parent->child_position.y += y_size;
    }

    if (flags & UIFlags_Visible) {
        Entity *entity = ui_get_entity(ui_manager);
        float screen_x = ui_manager->ctx->width;
        float screen_y = ui_manager->ctx->height;
        float pos_x = 2*(result->rect.x / screen_x) - 1;
        float pos_y = 1 - 2*(result->rect.y / screen_y);
        float pos_z = 2*result->rect.z / screen_x;
        float pos_w = -2*result->rect.w / screen_y;
        Vec4 position = newVec4(pos_x, pos_y, pos_z, pos_w);

        ui_entity_update(entity, position, color_a, color_b, color_c, color_d);
        result->entity = entity;
    }

    if (flags & UIFlags_Clickable) {
        float mouse[2] = {
            (float) ui_manager->ctx->mouse_position[0],
            (float) ui_manager->ctx->mouse_position[1]
        };
        if (
            (result->rect.x <= mouse[0]) &&
            (mouse[0] <= result->rect.x + result->rect.z) &&
            (result->rect.y <= mouse[1]) &&
            (mouse[1] <= result->rect.y + result->rect.w)
        ) {
            result->hot_t = 1.0;
        }
        else {
            result->hot_t = 0.0;
        }
        if ((result->hot_t > 0.0) && (glfwGetMouseButton(
            ui_manager->ctx->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
        ) {
            result->active_t = 1.0;
        }
        else {
            int hit = (result->hot_t > 0.0 && result->active_t > 0.0) ? 1: 0;
            result->hit = hit;
            result->active_t = 0.0;
        }
    }
    if (text) {
        _font_buffer_push(
            ui_manager->renderer->font, text,
            newVec2(
                result->rect.x + 0.1*result->rect.z,
                result->rect.y + 0.9*result->rect.w
            ),
            font_color
        );
    }

    return result;
}

UIWidget *ui_push_child_v(
    UIManager *ui_manager, float width, float height, int visible, Vec4 color
) {
    return ui_push_child_(
        ui_manager, width, height, 0, 1, visible, color, color, color, color,
        0, newVec3(0, 0, 0));
}

UIWidget *ui_push_child_h(
    UIManager *ui_manager, float width, float height, int visible, Vec4 color
) {
    return ui_push_child_(
        ui_manager, width, height, 1, 0, visible, color, color, color, color,
        0, newVec3(0, 0, 0));
}


void ui_padding(UIManager *ui_manager, Vec2 value, int relative) {
    Vec2 d_value = value;
    UIWidget *parent = ui_manager->current_parent_widget;
    if (relative) {
        d_value.x *= parent->rect.z;
        d_value.y *= parent->rect.w;
    }
    if (
        (d_value.x >= parent->rect.x + parent->rect.z) ||
        (d_value.y >= parent->rect.y + parent->rect.w)
    ) {
        printf("[WARNING] UI child is out of screen\n");
    }
    parent->child_position = newVec2(
        parent->child_position.x + d_value.x,
        parent->child_position.y + d_value.y
    );
}

void ui_set_child_position(UIManager *ui_manager, Vec2 value, int relative) {
    Vec2 d_value = value;
    UIWidget *parent = ui_manager->current_parent_widget;
    if (relative) {
        d_value.x *= parent->rect.z;
        d_value.y *= parent->rect.w;
    }
    if (
        (d_value.x >= parent->rect.x + parent->rect.z) ||
        (d_value.y >= parent->rect.y + parent->rect.w)
    ) {
        printf("[WARNING] UI child is out of screen\n");
    }
    parent->child_position = d_value;
}

void ui_push_parent(UIManager *ui_manager, UIWidget *widget) {
    ui_manager->current_parent_widget = widget;
    ui_manager->current_child_widget = 0;
}

UIWidget *ui_pop_parent(UIManager *ui_manager) {
    UIWidget *result = ui_manager->current_parent_widget;
    if (result->parent) {
        ui_manager->current_parent_widget = result->parent;
    }
    return result;
}

UIWidget *ui_push_child_null(UIManager *manager, float width, float height) {
    return ui_push_child_(manager, width, height, 0, 0, UIFlags_Clickable,
        newVec4(1.0, 1.0, 1.0, 1.0), newVec4(1.0, 1.0, 1.0, 1.0),
        newVec4(1.0, 1.0, 1.0, 1.0), newVec4(1.0, 1.0, 1.0, 1.0),
        0, newVec3(0, 0, 0));
}

typedef struct UIButtonColor {
    Vec4 color_a;
    Vec4 color_b;
    Vec4 color_c;
    Vec4 color_d;
} UIButtonColor;

UIButtonColor ui_button_get_colors(UIWidget *button, int pulse) {
    UIButtonColor result = {0};
    float top_color = 0.4;
    float button_color = 0.3;

    if (button->active_t) {
        top_color = 0.3;
        button_color = 0.4;
    }
    else if (button->hot_t > 0.0) {
        top_color = 0.45;
        button_color = 0.35;
    }

    result.color_a = newVec4(top_color, top_color, top_color, 1.0);
    result.color_b = newVec4(button_color, button_color, button_color, 1.0);
    result.color_c = newVec4(button_color, button_color, button_color, 1.0);
    result.color_d = newVec4(top_color, top_color, top_color, 1.0);

    return result;
}

int ui_button_widget(
    UIManager *ui_manager, float width, float height, char *text, Vec3 b_col
) {
    int result = 0;
    UIWidget *container = ui_push_child_null(ui_manager, width, height);
    ui_push_parent(ui_manager, container);
    if (container->hit){
        result = 1;
    }
    UIButtonColor color = ui_button_get_colors(container, result);
    ui_push_child_(ui_manager, 1.0, 1.0,
        0, 0, 1, color.color_a, color.color_b, color.color_c, color.color_d,
        text, b_col);
    ui_pop_parent(ui_manager);
    return result;
}

int ui_button_v(
    UIManager *ui_manager, float width, float height, char *text, Vec3 b_col
) {
    int result = ui_button_widget(ui_manager, width, height, text, b_col);
    ui_padding(ui_manager, newVec2(0.0, height), 1);
    return result;
}

int ui_button_h(
    UIManager *ui_manager, float width, float height, char *text, Vec3 b_col
) {
    int result = ui_button_widget(ui_manager, width, height, text, b_col);
    ui_padding(ui_manager, newVec2(width, 0.0), 1);
    return result;
}

int ui_toggle_widget(
    UIManager *ui_manager, float width, float height, int value
) {
    int result = value;
    float shade = 0.2;
    Vec4 color = newVec4(shade, shade, shade, 1.0);
    UIWidget *container = ui_push_child_(
        ui_manager, width, height, 0, 0,
        UIFlags_Visible|UIFlags_Square|UIFlags_Clickable,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, container);
    if (container->hit){
        result = 1 - result;
    }
    if (result > 0) {
        float padding = 0.08;
        float val = 1.0 - 2*padding;
        float fill = 0.5;
        Vec4 col = newVec4(fill, fill, fill, 1.0);
        ui_padding(ui_manager, newVec2(padding, padding), 1);
        ui_push_child_(ui_manager, val, val, 0, 0, 1, col, col, col, col,
            0, newVec3(0, 0, 0));
    }
    ui_pop_parent(ui_manager);
    return result;
}

int ui_toggle_v(UIManager *ui_manager, float width, float height, int value) {
    int result = ui_toggle_widget(ui_manager, width, height, value);
    ui_padding(ui_manager, newVec2(0.0, height), 1);
    return result;
}

int ui_toggle_h(UIManager *ui_manager, float width, float height, int value) {
    int result = ui_toggle_widget(ui_manager, width, height, value);
    ui_padding(ui_manager, newVec2(width, 0.0), 1);
    return result;
}

float ui_slider_h(
    UIManager *ui_manager, float width, float height, float value
) {
    float result = value;
    float shade = 0.2;
    Vec4 color = newVec4(shade, shade, shade, 1.0);
    UIWidget *container = ui_push_child_(
        ui_manager, width, height, 0, 0, UIFlags_Visible|UIFlags_Clickable,
        color, color, color, color, 0, newVec3(0, 0, 0));
    ui_push_parent(ui_manager, container);
    float mouse[2] = {
        (float) ui_manager->ctx->mouse_position[0],
        (float) ui_manager->ctx->mouse_position[1]
    };
    if (container->active_t > 0.0) {
        result = (mouse[0] - container->rect.x) / container->rect.z;
    }

    float padding_x = 0.02;
    float padding_y = 0.08;
    float val_x = result - padding_x;
    float val_y = 1.0 - 2*padding_y;

    if (val_x < 0.0) {
        val_x = 0.0;
        result = 0.0;
    }
    if (val_x >= (1.0 - 2*padding_x)) {
        result = 1.0;
        val_x = 1.0 - 2*padding_x;
    }
    float fill = 0.5;
    Vec4 col = newVec4(fill, fill, fill, 1.0);
    ui_padding(ui_manager, newVec2(padding_x, padding_y), 1);
    ui_push_child_(
        ui_manager, val_x, val_y, 0, 0, 1, col, col, col, col,
        0, newVec3(0, 0, 0));
    ui_pop_parent(ui_manager);
    return result;
}

float ui_slider_v(
    UIManager *ui_manager, float width, float height, float value
) {
    float result = value;
    float shade = 0.2;
    Vec4 color = newVec4(shade, shade, shade, 1.0);
    UIWidget *container = ui_push_child_(
        ui_manager, width, height, 0, 0, UIFlags_Visible|UIFlags_Clickable,
        color, color, color, color, 0, newVec3(0, 0, 0));
    ui_push_parent(ui_manager, container);
    float mouse[2] = {
        (float) ui_manager->ctx->mouse_position[0],
        (float) ui_manager->ctx->mouse_position[1]
    };
    if (container->active_t > 0.0) {
        result = (mouse[1] - container->rect.y) / container->rect.w;
    }
    float padding_x = 0.08;
    float padding_y = 0.02;
    float val_x = 1.0 - 2*padding_x;
    float val_y = result - padding_y;

    if (val_y < 0.0) {
        val_y = 0.0;
        result = 0.0;
    }
    if (val_y >= (1.0 - 2*padding_y)) {
        result = 1.0;
        val_y = 1.0 - 2*padding_y;
    }
    float fill = 0.5;
    Vec4 col = newVec4(fill, fill, fill, 1.0);
    ui_padding(ui_manager, newVec2(padding_x, padding_y), 1);
    ui_push_child_(ui_manager, val_x, val_y, 0, 0, 1, col, col, col, col,
        0, newVec3(0, 0, 0));
    ui_pop_parent(ui_manager);
    return result;
}


void scene_write_entity(char *buffer, int *cursor, Entity *entity) {
    *cursor += sprintf(buffer + *cursor, "Entity {\n");

    *cursor += sprintf(buffer + *cursor, "model %d\n", entity->model_name);
    *cursor += sprintf(
        buffer + *cursor, "position %f %f %f\n",
        entity->position.x, entity->position.y, entity->position.z
    );

    *cursor += sprintf(
        buffer + *cursor, "color %f %f %f\n",
        entity->color.x, entity->color.y, entity->color.z
    );

    *cursor += sprintf(
        buffer + *cursor, "rotation %f %f %f\n",
        entity->rotation.x, entity->rotation.y, entity->rotation.z
    );
    *cursor += sprintf(
        buffer + *cursor, "scale %f %f %f\n",
        entity->scale.x, entity->scale.y, entity->scale.z
    );
    *cursor += sprintf(buffer + *cursor, "wiremesh %d\n", entity->fill);

    *cursor += sprintf(buffer + *cursor, "}\n");
}

Vec3 load_vec3(Tokenizer *tokenizer) {
    Vec3 answer = {
        .x = atof(token_next(tokenizer).info),
        .y = atof(token_next(tokenizer).info),
        .z = atof(token_next(tokenizer).info)
    };
    return answer;
}

void scene_load(Renderer *renderer, Camera *camera) {
    Entity *entity = 0;
    for (int i=2; i<renderer->entities->counter; i++) {
        entity = LIST_GET_PTR(renderer->entities, i);
        entity->active = 0;
    }

    char *data = read_file("tmp_save.scene");

    Tokenizer tokenizer = {0};
    tokenizer.data = data;

    int loading_entity = 0;
    int loading_camera = 0;
    for (
        Token token = token_next(&tokenizer);
        tokenizer.data[tokenizer.cursor] && token_is_valid(&token);
        token = token_next(&tokenizer)
    ) {
        switch(token.kind) {
            case Token_Declaration: 
                if(strcmp(token.info, "Entity") == 0) {
                    assert(loading_entity == 0 && loading_camera == 0);
                    entity = get_entity(renderer);
                    loading_entity = 1;
                }
                if(strcmp(token.info, "Camera") == 0) {
                    assert(loading_entity == 0 && loading_camera == 0);
                    entity = get_entity(renderer);
                    loading_camera = 1;
                }
                if(strcmp(token.info, "Entity") == 0) {
                    entity = get_entity(renderer);
                    loading_entity = 1;
                }
                else if(strcmp(token.info, "model") == 0) {
                    int model = atoi(token_next(&tokenizer).info);
                    entity->model = renderer->models[model];
                    entity->model_name = model;
                }
                else if(strcmp(token.info, "position") == 0) {
                    Vec3 position = load_vec3(&tokenizer);

                    if (loading_entity) {
                        entity->position = position;
                    }
                    else if (loading_camera) {
                        camera->position = position;
                    }
                }
                else if(strcmp(token.info, "color") == 0) {
                    entity->color = load_vec3(&tokenizer);
                }
                else if(strcmp(token.info, "rotation") == 0) {
                    entity->rotation = load_vec3(&tokenizer);
                }
                else if(strcmp(token.info, "scale") == 0) {
                    entity->scale = load_vec3(&tokenizer);
                }
                else if(strcmp(token.info, "wiremesh") == 0) {
                    int fill = atoi(token_next(&tokenizer).info);
                    entity->fill = fill;
                }
                else if(strcmp(token.info, "pitch") == 0) {
                    assert(
                        loading_camera == 1 && "[ERROR LOADING] camera info"
                    );
                    camera->pitch = atof(token_next(&tokenizer).info);
                }
                else if(strcmp(token.info, "yaw") == 0) {
                    assert(
                        loading_camera == 1 && "[ERROR LOADING] camera info"
                    );
                    camera->yaw = atof(token_next(&tokenizer).info);
                }
            break;
            case Token_CloseCurl:
                assert( 
                    (loading_entity == 1 || loading_camera == 1) &&
                    "[ERROR LOADING] Scene info is corrupted"
                );
                if (loading_entity == 1) {
                    entity->active = 1;
                    loading_entity = 0;
                }
                else if (loading_camera == 1) {
                    loading_camera = 0;
                }
            break;
            default: 
            break;
        }
    }
    free(data);
}

void scene_save(Renderer *renderer, Camera *camera) {
    Entity *entity;

    char *save_buffer = malloc(10000 * sizeof(char));
    int cursor = 0;

    cursor += sprintf(save_buffer + cursor, "Camera {\n");
    cursor += sprintf(save_buffer + cursor, "pitch %f\n", camera->pitch);
    cursor += sprintf(save_buffer + cursor, "yaw %f\n", camera->yaw);
    cursor += sprintf(
        save_buffer + cursor, "position %f %f %f\n",
        camera->position.x, camera->position.y, camera->position.z
    );
    cursor += sprintf(
        save_buffer + cursor, "centre %f %f %f\n",
        camera->centre.x, camera->centre.y, camera->centre.z
    );

    cursor += sprintf(save_buffer + cursor, "}\n");

    for(int i=2; i<renderer->entities->counter; i++) {
        entity = LIST_GET_PTR(renderer->entities, i);
        scene_write_entity(save_buffer, &cursor, entity);
    }

    FILE *file = fopen("tmp_save.scene", "wb");
    fprintf(file, "%s", save_buffer);
    fclose(file);
}

void ui_test_button(UIManager *ui_manager, UI_InputParams *input) {
    Renderer *renderer = input->renderer;
    Camera *camera = input->camera;


    Vec4 color = newVec4(0.4, 0.4, 0.4, 0.65);
    UIWidget *panel = ui_push_child_(
        ui_manager, 0.3, 0.3, 1, 1, UIFlags_Clickable|UIFlags_Visible,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, panel);
    if (panel->hot_t) {
        editor_enter_mode(input->state, EditorMode_UI);
    }
    else {
        editor_exit_mode(input->state, EditorMode_UI);
    }
    ui_padding(ui_manager, newVec2(0.05, 0.05), 1);
    Vec3 yellow = newVec3(1.0, 1.0, 0.0);
    if (ui_button_v(ui_manager, 0.7, 0.2, "SAVE", yellow)) {
        scene_save(renderer, camera);
    }

    ui_padding(ui_manager, newVec2(0.0, 0.05), 1);
    if (ui_button_v(ui_manager, 0.7, 0.2, "LOAD", yellow)) {
        scene_load(renderer, camera);
    }
    ui_pop_parent(ui_manager);
}

void ui_test_button_1(UIManager *ui_manager) {
    static int toggle;
    static float slider;
    static float slider_r;
    static float slider_g;
    static float slider_b;
    UIWidget *first = ui_push_child_h(
        ui_manager, 0.5, 1.0, 1, newVec4(0.3, 0.3, 0.3, 0.5));
    ui_push_parent(ui_manager, first);

    ui_padding(ui_manager, newVec2(0.1, 0.1), 1);
    ui_push_child_v(ui_manager, 0.8, 0.1, 1, newVec4(1.0, 1.0, 1.0, 1.0));
    ui_padding(ui_manager, newVec2(0.0, 0.1), 1);
    toggle = ui_toggle_v(ui_manager, 0.1, 0.1, toggle);
    slider = ui_slider_h(ui_manager, 0.8, 0.1, slider);
    ui_padding(ui_manager, newVec2(0.0, 0.12), 1);

    UIWidget *third = ui_push_child_v(
        ui_manager, 1.0, 1.0, 0, newVec4(1.0, 1.0, 1.0, 1.0));
    ui_push_parent(ui_manager, third);
    
    slider_r = ui_slider_v(ui_manager, 0.08, 0.2, slider_r);
    ui_padding(ui_manager, newVec2(0.12, 0.0), 1);
    slider_g = ui_slider_v(ui_manager, 0.08, 0.2, slider_g);
    ui_padding(ui_manager, newVec2(0.12, 0.0), 1);
    slider_b = ui_slider_v(ui_manager, 0.08, 0.2, slider_b);
    ui_pop_parent(ui_manager);

    ui_pop_parent(ui_manager);
    ui_padding(ui_manager, newVec2(0.1, 0.0), 1);
    ui_push_child_h(ui_manager, 0.25, 0.25, 1,
        newVec4(slider_r, slider_g, slider_b, 1.0));

    ui_set_child_position(ui_manager, newVec2(0.51, 0.51), 1);
    UIWidget *second = ui_push_child_v(ui_manager, 0.5, 0.5, 1, 
        newVec4(0.4, 0.4, 0.4, 1.0)
    );
    ui_push_parent(ui_manager, second);
    ui_padding(ui_manager, newVec2(0.1, 0.1), 1);
    ui_push_child_v(ui_manager, 0.8, 0.2, 1, 
        newVec4(0.6, 0.6, 0.6, 1.0)
    );
    ui_padding(ui_manager, newVec2(0.0, 0.1), 1);
    if (ui_button_v(ui_manager, 0.8, 0.2, 0, newVec3(1.0, 1.0, 0.0))) {
        ui_push_child_v(ui_manager, 0.8, 0.2, 1, 
            newVec4(0.6, 0.6, 0.6, 1.0)
        );
    }

    printf("We have %lu gui entities\n", ui_manager->gui_entities->counter);
}

void editor_enter_mode(EditorState *state, EditorMode new_mode) {
    if (state->state != new_mode) {
        state->prev_state = state->state;
    }
    state->state = new_mode;
}

void editor_exit_mode(EditorState *state, EditorMode exit_mode) {
    if (state->prev_state == exit_mode) {
        state->prev_state = EditorMode_None;
    }
    if (state->state == exit_mode) {
        state->state = state->prev_state;
    }
}

Entity *get_layer_entity(List(Entity) *entity_list, int i) {
    while(entity_list->counter < 10) {
        list_push(entity_list, Entity);
    }
    return LIST_GET_PTR(entity_list, i);
}

float get_layer_entity_scale(int i) {
    float options[] = {
        0.1, 0.04, 0.015, 0.04,
        0.02, 0.005, 0.02, 0.02,
        0.015,
    };
    return options[i];
}

float get_layer_entity_offset(int i) {
    float options[] = {
        0.75, 0.75, 0.75, 0.5,
        0.5, 0.75, 0.5, 0.5,
        0.95
    };
    return options[i];
}
void add_selection_entity(UIManager *ui_manager, Camera *camera, int i) {
    Entity *entity = get_layer_entity(
        ui_manager->renderer->layers->entities, i);
    entity->model = ui_manager->renderer->models[i+1];
    float scale = get_layer_entity_scale(i);
    entity->scale = newVec3(scale, scale, scale);
    entity->active = 1;
    UIWidget *button = ui_manager->current_child_widget;
    Vec3 pos = screen_to_world(
        ui_manager->ctx, ui_manager->renderer, camera,
        newVec2(
            button->rect.x + 0.5 * button->rect.z,
            button->rect.y + get_layer_entity_offset(i) * button->rect.w
        ));
    pos = vec3_add(&camera->position, &pos);
    entity->position = pos;
}


void ui_pick_entity(UIManager *ui_manager, UI_InputParams *input) {
    Camera *camera = input->camera;

    Vec4 color = newVec4(0.4, 0.4, 0.4, 1.0);
    ui_manager->current_parent_widget->child_position = newVec2(0.0, 0.0);
    ui_padding(ui_manager, newVec2(0.0, 0.6), 1);
    UIWidget *panel = ui_push_child_(
        ui_manager, 0.3, 0.4, 1, 1, UIFlags_Clickable|UIFlags_Visible,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, panel);
    if (panel->hot_t) {
        editor_enter_mode(input->state, EditorMode_UI);
    }
    else {
        editor_exit_mode(input->state, EditorMode_UI);
    }

    color = newVec4(1.0, 0.0, 0.0, 1.0);
    ui_padding(ui_manager, newVec2(0.00, 0.02), 1);
    UIWidget *layout = ui_push_child_(
        ui_manager, 1.0, 0.3, 0, 1, 0,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, layout);

    Vec3 yellow = newVec3(1.0, 1.0, 0.0);
    for(int i=0; i<4; i++) {
        ui_padding(ui_manager, newVec2(0.04, 0.01), 1);
        int selection = ui_button_h(ui_manager, 0.2, 0.9, 0, yellow);
        add_selection_entity(ui_manager, camera, i);
        if (selection) {
            *input->selection = i+1;
        }
    }

    ui_pop_parent(ui_manager);
    ui_padding(ui_manager, newVec2(0.00, 0.02), 1);
    layout = ui_push_child_(
        ui_manager, 1.0, 0.30, 0, 1, 0,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, layout);
    for(int i=0; i<4; i++) {
        ui_padding(ui_manager, newVec2(0.04, 0.00), 1);
        int selection = ui_button_h(ui_manager, 0.2, 0.9, 0, yellow);
        add_selection_entity(ui_manager, camera, i+4);
        if (selection) {
            *input->selection = i+1+4;
        }
    }

    ui_pop_parent(ui_manager);
    ui_padding(ui_manager, newVec2(0.00, 0.01), 1);
    layout = ui_push_child_(
        ui_manager, 1.0, 0.3, 0, 1, 0,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, layout);
    ui_padding(ui_manager, newVec2(0.04, 0.00), 1);
    int selection = ui_button_h(ui_manager, 0.2, 0.9, 0, yellow);
    add_selection_entity(ui_manager, camera, 8);
    if (selection) {
        *input->selection = 9;
    }

    layout = ui_push_child_(
        ui_manager, 0.7, 1.0, 0, 1, 0,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, layout);
    ui_padding(ui_manager, newVec2(0.2, 0.5), 1);
    int edit_mode = ui_button_h(ui_manager, 0.8, 0.5, "Edit Mode", yellow);
    if (edit_mode) {
        input->state->default_state = EditorMode_EditEntity;
        input->state->state = EditorMode_EditEntity;
        RenderLayer *layer = ui_manager->renderer->layers;
        for(int i=0; i<layer->entities->counter; i++) {
            Entity *entity = LIST_GET_PTR(layer->entities, i);
            entity->active = 0;
        }
    }

    ui_pop_parent(ui_manager);

    ui_pop_parent(ui_manager);
    ui_pop_parent(ui_manager);

}

void ui_edit_entity(UIManager *ui_manager, UI_InputParams *input) {
    Vec4 color = newVec4(0.4, 0.4, 0.4, 1.0);
    ui_manager->current_parent_widget->child_position = newVec2(0.0, 0.0);
    ui_padding(ui_manager, newVec2(0.0, 0.5), 1);
    UIWidget *panel = ui_push_child_(
        ui_manager, 0.3, 0.5, 1, 1, UIFlags_Clickable|UIFlags_Visible,
        color, color, color, color, 0, newVec3(0, 0, 0)
    );
    ui_push_parent(ui_manager, panel);
    if (panel->hot_t) {
        editor_enter_mode(input->state, EditorMode_UI);
    }
    else {
        editor_exit_mode(input->state, EditorMode_UI);
    }


    Vec3 yellow = newVec3(1.0, 1.0, 0.0);
    ui_padding(ui_manager, newVec2(0.5, 0.85), 1);
    int create_mode = ui_button_h(
        ui_manager, 0.45, 0.1, "Create Mode", yellow);
    if (create_mode) {
        input->state->default_state = EditorMode_CreateEntity;
        input->state->state = EditorMode_CreateEntity;
        input->state->selected_entity = 0;
    }
    ui_pop_parent(ui_manager);

}


