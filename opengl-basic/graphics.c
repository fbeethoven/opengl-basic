#include "graphics.h"
#include "common.h"
#include "shader.h"
#include "image.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void camera_move(Camera *camera, float dx, float dy, float dz) {
    camera->position.x += dx;
    camera->position.y += dy;
    camera->position.z += dz;
}

void store_float_in_attributes(
    unsigned int *buffer_id,
    int attribute_index,
    int coordinate_size,
    int buffer_size,
    float *data
) {
    glGenBuffers(1, buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, *buffer_id);
    glBufferData(
        GL_ARRAY_BUFFER,
        buffer_size,
        data,
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(
        attribute_index, coordinate_size, GL_FLOAT,
        GL_FALSE, coordinate_size * sizeof(float), 0
    );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void bind_indices_buffer(
    unsigned int *buffer_id,
    int buffer_size,
    unsigned int *data
) {
    glGenBuffers(1, buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer_id);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        buffer_size,
        data,
        GL_STATIC_DRAW
    );
}


void load_data_to_model(
    BaseModel *model,
    float *vertices, unsigned int *indices,
    int vertices_size, int indices_size
) {
    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);

    bind_indices_buffer(&model->ibo, indices_size, indices);

    store_float_in_attributes(
        &model->vbo, 0, 3, vertices_size, vertices
    );

    glBindVertexArray(0);
}

void load_empty_texture_to_model(
    BaseModel *model, float *texture_coord, int textures_size
){
    glBindVertexArray(model->vao);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &model->texture_id);
    glBindTexture(GL_TEXTURE_2D, model->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int texture[256*256];
    for (int i=0; i<256*256; i++) {
        texture[i] = 0xffffffff;
    }

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGB,
        GL_UNSIGNED_BYTE, texture
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    store_float_in_attributes(
        &model->uv, 1, 2, textures_size, texture_coord
    );

    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}


void load_texture_to_model(
    BaseModel *model, char *texture_file_path,
    float *texture_coord, int textures_size
){
    glBindVertexArray(model->vao);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &model->texture_id);
    glBindTexture(GL_TEXTURE_2D, model->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Image *test = image_load(texture_file_path);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, test->width, test->height,
        0, GL_RGB, GL_UNSIGNED_BYTE, test->data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    image_free(test);
    store_float_in_attributes(
        &model->uv, 1, 2, textures_size, texture_coord
    );

    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}


int graphics_init(GraphicsContext *ctx) {
    ctx->width = 800;
    ctx->height = 600;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        ctx->width, ctx->height, "LearnOpenGL", NULL, NULL
    );
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }
    ctx->window = window;
    glfwGetCursorPos(
        ctx->window, &ctx->mouse_position[0], &ctx->mouse_position[1]
    );
    return 0;
}


static Mat4 create_projection_matrix(
    GraphicsContext *ctx, Renderer* rh
) {
    float aspectRatio = (float)ctx->width / (float)ctx->height;
    float y_scale = (float) ((1.0f / tan(rh->FOV / 2.0f)) * aspectRatio);
    float x_scale = y_scale / aspectRatio;
    float frustum_length = rh->FAR_PLANE - rh->NEAR_PLANE;

    Mat4 projectionMatrix = Mat4I();
    projectionMatrix.m00 = x_scale;
    projectionMatrix.m11 = y_scale;
    projectionMatrix.m22 = -(
        (rh->FAR_PLANE + rh->NEAR_PLANE) / frustum_length
    );
    projectionMatrix.m23 = -1;
    projectionMatrix.m32 = -(
        (2 * rh->NEAR_PLANE * rh->FAR_PLANE) / frustum_length
    );
    projectionMatrix.m33 = 0;
    return projectionMatrix;
}


void reload_projection_matrix(GraphicsContext *ctx, Renderer *rh) {
    rh->projection_matrix = create_projection_matrix(ctx, rh);
    shader_push(rh->shader);
    shader_load_matrix(
        rh->shader,
        "projection_matrix",
        &rh->projection_matrix
    );
    shader_push(rh->circle_shader);
    shader_load_matrix(
        rh->circle_shader,
        "projection_matrix",
        &rh->projection_matrix
    );
    shader_pop();
}


void init_render_handler(GraphicsContext *ctx, Renderer *rh) {
    rh->FOV = 1.19377;
	rh->NEAR_PLANE = 0.1f;
	rh->FAR_PLANE = 1000;

	rh->RED = 0.4f;
	rh->GREEN = 0.4f;
	rh->BLUE = 0.4f;

    rh->projection_matrix = create_projection_matrix(ctx, rh);

    rh->shader = shader_get_program();
    rh->circle_shader = shader_get_program_general(
        "shaders/circle_vertex.glsl", "shaders/circle_fragment.glsl"
    );
    rh->gui_shader = shader_get_program_general(
        "shaders/gui_vertex.glsl", "shaders/gui_fragment.glsl"
    );
    log_if_err("There was an issue on renderer init\n");

    shader_push(rh->shader);
    shader_load_matrix(
        rh->shader,
        "projection_matrix",
        &rh->projection_matrix
    );
    shader_push(rh->circle_shader);
    shader_load_matrix(
        rh->circle_shader,
        "projection_matrix",
        &rh->projection_matrix
    );
    shader_pop();
}


void prepare(Renderer *rh) {
    log_if_err("There was an issue BEFORE Preparing\n");
    glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(rh->RED, rh->GREEN, rh->BLUE, 1);
    log_if_err("There was an issue Preparing\n");
}

void render_entities(Renderer *rh) {
    Vec3 light_color = rh->light->color;
    for (int i=0; i<20; i++) {
        Entity entity = rh->entities[i];

        if (entity.active == 0) {
            continue;
        }
        if (
            !vec3_is_equal(entity.color, newVec3(0.0, 0.0, 0.0)) &&
            !vec3_is_equal(entity.color, light_color)
        ) {
            log_if_err(
                "Entity Renderer found an issue before loading light\n"
            );
            rh->light->color = entity.color;
            shader_load_light(rh->shader, rh->light);
            log_if_err("Entity Renderer found problem loading lights");
        }

        glBindVertexArray(entity.model->vao);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        log_if_err("There was an issue with attributes\n");
        
        Mat4 transformation_matrix = create_transformation_matrix(
            entity.position,
            entity.rotation_x,
            entity.rotation_y,
            entity.rotation_z,
            &entity.scale
        );
        shader_load_matrix(
            rh->shader,
            "transformation_matrix",
            &transformation_matrix
        );

        if ( (entity.fill & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entity.model->texture_id);
        glDrawElements(
            GL_TRIANGLES, entity.model->vertex_count,
            GL_UNSIGNED_INT, 0
        );

        if (!vec3_is_equal(entity.color, light_color)) {
            log_if_err("Issue before loading light\n");
            rh->light->color = light_color;
            shader_load_light(rh->shader, rh->light);
            log_if_err("There was a problem loading lights");
        }
	}

    for (int i=0; i<100; i++) {
        Entity entity = rh->debug_entities[i];

        if (entity.active == 0) {
            continue;
        }
        if (
            !vec3_is_equal(entity.color, newVec3(0.0, 0.0, 0.0)) &&
            !vec3_is_equal(entity.color, light_color)
        ) {
            log_if_err(
                "Entity Renderer found an issue before loading light\n"
            );
            rh->light->color = entity.color;
            shader_load_light(rh->shader, rh->light);
            log_if_err("Entity Renderer found problem loading lights");
        }

        glBindVertexArray(entity.model->vao);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        log_if_err("There was an issue with attributes\n");
        
        Mat4 transformation_matrix = create_transformation_matrix(
            entity.position,
            entity.rotation_x,
            entity.rotation_y,
            entity.rotation_z,
            &entity.scale
        );
        shader_load_matrix(
            rh->shader,
            "transformation_matrix",
            &transformation_matrix
        );

        if ( (entity.fill & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entity.model->texture_id);
        glDrawElements(
            GL_TRIANGLES, entity.model->vertex_count,
            GL_UNSIGNED_INT, 0
        );

        if (!vec3_is_equal(entity.color, light_color)) {
            log_if_err("Issue before loading light\n");
            rh->light->color = light_color;
            shader_load_light(rh->shader, rh->light);
            log_if_err("There was a problem loading lights");
        }
	}

    // shader_push(rh->circle_shader);
    // for (int i=10; i<20; i++) {
    //     Entity entity = rh->entities[i];

    //     if (entity.active == 0) {
    //         continue;
    //     }
    //     glBindVertexArray(entity.model->vao);

    //     glEnableVertexAttribArray(0);
    //     glEnableVertexAttribArray(1);
    //     glEnableVertexAttribArray(2);
    //     printf("ENTITY: %s\n", entity.debug_name);
    //     log_if_err("There was an issue with attributes\n");
    //     
    //     Mat4 transformation_matrix = create_transformation_matrix(
    //         entity.position,
    //         entity.rotation_x,
    //         entity.rotation_y,
    //         entity.rotation_z,
    //         &entity.scale
    //     );
    //     shader_load_matrix(
    //         rh->circle_shader,
    //         "transformation_matrix",
    //         &transformation_matrix
    //     );
    //     shader_load_vec3(
    //         rh->circle_shader,
    //         "color",
    //         &entity.color
    //     );

    //     if ( (entity.fill & 1) == 0) {
    //         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //     }
    //     else {
    //         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //     }

    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, entity.model->texture_id);
    //     glDrawElements(
    //         GL_TRIANGLES, entity.model->vertex_count,
    //         GL_UNSIGNED_INT, 0
    //     );
	// }
}


void render_font_entities(Renderer *rh) {
    for (int i=0; i<10; i++) {
        Entity entity = rh->font_entities[i];

        if (entity.active == 0) {
            continue;
        }
        log_if_err("Issue before gui entities\n");

        glBindVertexArray(entity.model->vao);
        log_if_err("Issue bindin Vertex Array\n");

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        log_if_err("Issue with Vertex Attribs\n");

        log_if_err("Issue before subdata\n");
        glBindBuffer(GL_ARRAY_BUFFER, entity.model->vbo);
        log_if_err("Issue while binding buffer\n");

        font_update_buffer(rh->font);

        Mat4 transformation_matrix = create_transformation_matrix(
            entity.position,
            entity.rotation_x,
            entity.rotation_y,
            entity.rotation_z,
            &entity.scale
        );
        shader_load_matrix(
            rh->gui_shader,
            "transformation_matrix",
            &transformation_matrix
        );
        
        if ( (entity.fill & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        log_if_err("Font ussue with Polygons\n");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rh->font->texture_id);
        log_if_err("Issue before drawing font entities\n");

        glDrawElements(
            GL_TRIANGLES, rh->font->font_mesh->indices_len,
            GL_UNSIGNED_INT, 0
        );
        log_if_err("Issue after font entities\n");
	}
}


void render_gui_entities(Renderer *rh) {
    for (int i=0; i<10; i++) {
        Entity entity = rh->gui_entities[i];

        if (entity.active == 0) {
            continue;
        }
        log_if_err("Issue before gui entities\n");

        glBindVertexArray(entity.model->vao);
        log_if_err("Issue bindin Vertex Array\n");

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        log_if_err("Issue with Vertex Attribs\n");
        glBindBuffer(GL_ARRAY_BUFFER, entity.model->vbo);

        Mat4 transformation_matrix = create_transformation_matrix(
            entity.position,
            entity.rotation_x,
            entity.rotation_y,
            entity.rotation_z,
            &entity.scale
        );

        shader_load_matrix(
            rh->gui_shader,
            "transformation_matrix",
            &transformation_matrix
        );
        
        if ( (entity.fill & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        log_if_err("Gui issue with Polygons\n");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entity.model->texture_id);
        log_if_err("Issue before drawing gui entities\n");
        glDrawElements(
            GL_TRIANGLES, entity.model->vertex_count,
            GL_UNSIGNED_INT, 0
        );
        log_if_err("Issue after gui entities\n");
	}
}


void render(Renderer *rh, Camera *camera) {
    prepare(rh);

    Mat4 view_matrix = mat4_look_at(
        camera->position, 
        camera->centre,
        newVec3(0.0, 1.0, 0.0)
    );
    shader_push(rh->circle_shader);
    shader_load_matrix(
        rh->circle_shader,
        "view_matrix",
        &view_matrix
    );
    log_if_err("Renderer found a problem with circle shader\n");
    shader_push(rh->shader);
    shader_load_matrix(
        rh->shader,
        "view_matrix",
        &view_matrix
    );
    log_if_err("Renderer found a problem with shader\n");

    log_if_err("Renderer found an issue before loading light\n");
    shader_load_light(rh->shader, rh->light);
    log_if_err("Renderer found problem loading lights");

    render_entities(rh);

    glDisableVertexAttribArray(2);
    glDisable(GL_DEPTH_TEST);

    shader_push(rh->gui_shader);

    render_gui_entities(rh);
    render_font_entities(rh);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    shader_pop();

    log_if_err("Issue after pipeline\n");
}


void cleanUp(Renderer *rh) {
    glDeleteProgram(rh->shader);
}


void increase_position(Entity *entity, float dx, float dy, float dz) {
    entity->position->x += dx;
    entity->position->y += dy;
    entity->position->z += dz;
}


void increase_rotation(Entity *entity, float dx, float dy, float dz) {
    entity->rotation_x += dx;
    entity->rotation_y += dy;
    entity->rotation_z += dz;
}


void joint_update(Joint *parent, Joint *joint, int is_root) {
    Entity *entity = joint->entity;

    if (is_root) {
        *entity->position = joint->local_transform.translation;
        entity->rotation_x = joint->local_transform.rotation.x;
        entity->rotation_y = joint->local_transform.rotation.y;
        entity->rotation_z = joint->local_transform.rotation.z;
        return;
    }
    if (!parent) { return; }

    Entity *e_parent = parent->entity;
    Vec3 scale = newVec3(1.0, 1.0, 1.0);
    Mat4 parent_transformation = create_transformation_matrix(
        e_parent->position,
        e_parent->rotation_x, e_parent->rotation_y, e_parent->rotation_z,
        &scale
    );
    parent_transformation = mat4_transpose(&parent_transformation);

    Vec4 transform = newVec4(
        joint->local_transform.translation.x,
        joint->local_transform.translation.y,
        joint->local_transform.translation.z,
        1.0
    );
    transform = vec4_multiply(&parent_transformation, &transform);
    *entity->position = newVec3(transform.x, transform.y, transform.z);

    entity->rotation_x = (
        e_parent->rotation_x + joint->local_transform.rotation.x
    );
    entity->rotation_y = (
        e_parent->rotation_y + joint->local_transform.rotation.y
    );
    entity->rotation_z = (
        e_parent->rotation_z + joint->local_transform.rotation.z
    );
}

void joint_update_children(Joint *root, int is_root) {
    joint_update(0, root, is_root);
    for (
        Joint *current = root->children;
        current;
        current = current->next
    ) {
        current->entity->position->y = 1.0;
        joint_update(root, current, 0);
        joint_update_children(current, 0);
    }
}

void joint_update_all(Joint *root) {
    joint_update_children(root, 1);
}

Joint *new_joint(Entity *entity) {
    Joint *joint = calloc(sizeof(Joint), 1);
    joint->entity = entity;
    return joint;
}

Joint *joint_push(Joint *joint, Entity *entity) {
    Joint *child = new_joint(entity);
    child->next = joint->children;
    joint->children = child;
    return child;
}
