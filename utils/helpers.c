#include "helpers.h"


void gui_quad_in_pos(
    GraphicsContext *ctx, Entity *entity, Vec2 center, float size, Vec3 color
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

    float pos_x = (2 * center.x/(float)ctx->width) - 1.0;
    float pos_y = 1.0 - (2 * center.y/(float)ctx->height);
    Vec3 *rect_pos = (Vec3 *)malloc(sizeof(Vec3));
    *rect_pos = newVec3(pos_x, pos_y, 0);

    entity->position = rect_pos;
    entity->active = 1;
    entity->scale = 1.0;
}


void gui_quad_free(Entity *entity) {
    free(entity->model);
    free(entity->position);
}


void fps_player_movement(GraphicsContext *ctx, CameraMovementParams *params) {
    double cursor_x, cursor_y;
    glfwGetCursorPos(ctx->window, &cursor_x, &cursor_y);

    double dx = cursor_x - ctx->mouse_position[0];
    double dy = cursor_y - ctx->mouse_position[1];

    ctx->mouse_position[0] = cursor_x;
    ctx->mouse_position[1] = cursor_y;

    glfwSetCursorPos(
        ctx->window, 0.5*(double)ctx->width, 0.5*(double)ctx->height
    );


    Camera *camera = params->camera;

    // if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
    //     increase_rotation(params->player, 0.0, params->rotation_factor, 0.0);
    // }
    // if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
    //     increase_rotation(params->player, 0.0, -params->rotation_factor, 0.0);
    // }
    // if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
    //     params->d_player_move += params->speed;
    // }
    // if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
    //     params->d_player_move -= params->speed;
    // }

    // params->player->position->x += (
    //     params->d_player_move * sinf(params->player->rotation_y)
    // );
    // params->player->position->z += (
    //     params->d_player_move * cosf(params->player->rotation_y)
    // );

}


void player_movement(GraphicsContext *ctx, PlayerMovementParams *params) {
    if (glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        increase_rotation(params->player, 0.0, params->rotation_factor, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        increase_rotation(params->player, 0.0, -params->rotation_factor, 0.0);
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        params->d_player_move += params->speed;
    }
    if (glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        params->d_player_move -= params->speed;
    }

    params->player->position->x += (
        params->d_player_move * sinf(params->player->rotation_y)
    );
    params->player->position->z += (
        params->d_player_move * cosf(params->player->rotation_y)
    );
}


void camera_follow_player(Entity *player, CameraMovementParams *params) {
    float horizontal_distance = (
        params->distance_from_player * cosf(params->camera->pitch)
    );
    float vertical_distance = (
        params->distance_from_player * sinf(params->camera->pitch)
    );
    float theta = params->camera->yaw + player->rotation_y;

    float target_x = player->position->x - horizontal_distance*sinf(theta);
    float target_z = player->position->z - horizontal_distance*cos(theta);
    float target_y = player->position->y + vertical_distance;

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
    params->camera->centre = *player->position;

    // camera->position.x = player->position->x - horizontal_distance*sinf(theta);
    // camera->position.z = player->position->z - horizontal_distance*cos(theta);
    // camera->position.y = player->position->y + vertical_distance;

    // camera->centre = *player->position;
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


void draw_quad_in_pixels(
    GraphicsContext *ctx, Mesh *mesh, Vec2 center, Vec3 color, float side
) {
    float x = 2 * (center.x/ctx->width) - 1;
    float y = 1 - 2 * (center.y/ctx->height);

    draw_quad(mesh, newVec3(x, y, 0), color, side/ctx->width);
}


