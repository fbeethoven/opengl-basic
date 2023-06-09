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
    }
    else {
        glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

Vec3 mouse_to_plane(
    GraphicsContext *ctx, Renderer *renderer, Camera *camera,
    Vec3 normal, float distance
) {
    Vec3 dir = mouse_to_world(ctx, renderer, camera);
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




// ui-test
// void attach_mesh_component(Entity *entity) {
//     if (!entity->components) {
//         entity->components = new_array_list(MeshComponent);
//     }
// 
//     MeshComponent *mesh = arr_push(entity->components, MeshComponent);
//     mesh->vertices = NEW_LIST(Vec3);
//     mesh->uvs = NEW_LIST(Vec2);
//     mesh->color = NEW_LIST(Vec3);
//     mesh->indices = NEW_LIST(int);
// 
//     BaseModel *model = entity->model;
// 
//     log_if_err("[MeshComponent] Issue before initialization");
//     glGenVertexArrays(1, &model->vao);
//     glBindVertexArray(model->vao);
// 
//     glGenBuffers(1, &model->ibo);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
//     U32 ibo_size = 6 * MeshCapacity * sizeof(U32);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo_size, 0, GL_DYNAMIC_DRAW);
// 
//     glGenBuffers(1, &model->vbo);
// 	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
//     U32 vbo_size = 3 * 4 * MeshCapacity * sizeof(float);
//     glBufferData(GL_ARRAY_BUFFER, vbo_size, 0, GL_DYNAMIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
//     log_if_err("[MeshComponent] Issue during initialization");
// 
//     glGenBuffers(1, &model->uv);
// 	glBindBuffer(GL_ARRAY_BUFFER, model->uv);
//     U32 uv_size = 2 * 4 * MeshCapacity * sizeof(float);
//     glBufferData(GL_ARRAY_BUFFER, uv_size, 0, GL_DYNAMIC_DRAW);
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
// 
//     glGenBuffers(1, &model->color);
// 	glBindBuffer(GL_ARRAY_BUFFER, model->color);
//     U32 color_size = 4 * 4 * MeshCapacity * sizeof(float);
//     glBufferData(GL_ARRAY_BUFFER, color_size, 0, GL_DYNAMIC_DRAW);
//     glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
//     log_if_err("[MeshComponent] Issue during initialization");
// }

// void mesh_component_update(Entity *entity) {
//     BaseModel *model = entity->model;
//     MeshComponent *mesh = arr_get_ptr(entity->components, MeshComponent, 0);  
// 
//     U32 vbo_size = mesh->vertices->counter * 3 * sizeof(float);
//     glBufferSubData(
//         GL_ARRAY_BUFFER, 0, vbo_size, (float *)mesh->vertices->data
//     );
//     glBindBuffer(GL_ARRAY_BUFFER, model->uv);
//     U32 uv_size = mesh->uvs->counter * 2 * sizeof(float);
//     glBufferSubData(GL_ARRAY_BUFFER, 0, uv_size, (float *)mesh->uvs->data);
//     glBindBuffer(GL_ARRAY_BUFFER, model->color);
//     U32 color_size = mesh->color->counter * 4 * sizeof(float);
//     glBufferSubData(
//         GL_ARRAY_BUFFER, 0, color_size, (float *)mesh->color->data
//     );
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ibo);
//     U32 ibo_size = mesh->indices->counter * 2 * sizeof(unsigned int)
//     glBufferSubData(
//         GL_ELEMENT_ARRAY_BUFFER, 0, ibo_size, (U32 *)mesh->indices->data
//     );
//     log_if_err("Issue with subdata\n");
// }


// Vec2 mesh_quad_push(MeshComponent *mesh, Vec2 position, Vec4 color) {
//     Vec2 size = {0};
//     Mesh *mesh = font->font_mesh;
//     float offsetX = position.x;
//     float offsetY = position.y;
//     int counter = mesh->vertices_len;
//     int indices_counter = mesh->indices_len;
//     char c;
//     for (int i=0; i<strlen(msg); i++) {
//         c = msg[i];
//         GlyphInfo glyph_info = getGlyphInfo(font, c, offsetX, offsetY);
//         offsetX = glyph_info.offsetX;
//         offsetY = glyph_info.offsetY;
// 
//         mesh->vertices[counter] = glyph_info.positions[0];
//         mesh->vertices[counter + 1] = glyph_info.positions[1];
//         mesh->vertices[counter + 2] = glyph_info.positions[2];
//         mesh->vertices[counter + 3] = glyph_info.positions[3];
// 
//         mesh->uvs[counter] = glyph_info.uvs[0];
//         mesh->uvs[counter + 1] = glyph_info.uvs[1];
//         mesh->uvs[counter + 2] = glyph_info.uvs[2];
//         mesh->uvs[counter + 3] = glyph_info.uvs[3];
// 
//         mesh->color[counter] = color;
//         mesh->color[counter + 1] = color;
//         mesh->color[counter + 2] = color;
//         mesh->color[counter + 3] = color;
// 
//         mesh->indices[indices_counter] = counter;
//         mesh->indices[indices_counter + 1] = counter + 1;
//         mesh->indices[indices_counter + 2] = counter + 2;
//         mesh->indices[indices_counter + 3] = counter;
//         mesh->indices[indices_counter + 4] = counter + 2;
//         mesh->indices[indices_counter + 5] = counter + 3;
// 
//         counter += 4;
//         indices_counter += 6;
// 
//         if (glyph_info.positions[2].y > size.y) {
//             size.y = glyph_info.positions[2].y;
//         }
//         size.x = glyph_info.positions[2].x;
//     }
// 
//     mesh->vertices_len = counter;
//     mesh->uvs_len = counter;
//     mesh->color_len = counter;
//     mesh->indices_len = indices_counter;
// 
// 
//     return newVec2(
//         (0.5 + 0.5*size.x) * font->width - position.x,
//         position.y - (0.5 - 0.5*size.y) * font->height
//     );
// }


