#include "helpers.h"


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


void free_camera_movement(GraphicsContext *ctx, CameraMovementParams *params) {
    float player_is_grounded = params->player_is_grounded;
    printf("We are using player_is_grounded: %f\n", player_is_grounded);
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

Vec3 ray_to_plane_from(Vec3 origin, Vec3 toward, Vec3 normal, float distance) {
    Vec3 dir = newVec3(
        toward.x - origin.x, toward.y - origin.y, toward.z - origin.z
    );
    return ray_to_plane(origin, dir, normal, distance);
}

Vec3 ray_to_plane(Vec3 origin, Vec3 dir, Vec3 normal, float distance) {
    Vec3 normalize_dir = dir;
    vec3_normalize(&normalize_dir);

    float t = vec3_dot(&normalize_dir, &normal);
    if (t == 0) { 
        return newVec3(0.0, 0.0, 0.0); 
    }
    t = (distance - vec3_dot(&origin, &normal)) / t;

    normalize_dir.x *= t;
    normalize_dir.y *= t;
    normalize_dir.z *= t;
    return vec3_add(&origin, &normalize_dir);
}

Vec3 mouse_to_plane(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera,
    Vec3 normal, float distance
) {
    float x = 2 * (float)ctx->mouse_position[0]/ctx->width - 1.0;
    float y = 1.0 - 2 * (float)ctx->mouse_position[1]/ctx->height;

    Vec4 rel_pos = newVec4(x, y, -1.0, 1.0);

    Mat4 projection_inverse = mat4_inverse(&renderer->projection_matrix);

    Mat4 view_matrix = mat4_look_at(
        camera->position, 
        camera->centre,
        newVec3(0.0, 1.0, 0.0)
    );
    view_matrix = mat4_inverse(&view_matrix);

    rel_pos = vec4_multiply(&projection_inverse, &rel_pos);
    rel_pos.z = -1.0;
    rel_pos.w = 0.0;
    rel_pos = vec4_multiply(&view_matrix, &rel_pos);

    Vec3 dir = newVec3(rel_pos.x, rel_pos.y, rel_pos.z);
    return ray_to_plane(camera->position, dir, normal, distance);
}


int ui_button(
    GraphicsContext *ctx, Renderer *renderer, Vec2 position, char *text
) {
    Vec3 button_color = newVec3(0.55, 0.55, 0.40);
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
        button_color = newVec3(0.66, 0.66, 0.6);
        font_color = newVec3(0.88, 0.88, 0.5);
        is_hot = 1;
    }
    _font_buffer_push(renderer->font, text, position, font_color);

    printf("Using Button Color: %f\n", button_color.x);
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
