#include "graphics.h"
#include "common.h"
#include "memory.h"
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

void store_int_in_attributes(
    unsigned int *buffer_id,
    int attribute_index,
    int coordinate_size,
    int buffer_size,
    int *data
) {
    glGenBuffers(1, buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, *buffer_id);
    glBufferData(
        GL_ARRAY_BUFFER,
        buffer_size,
        data,
        GL_STATIC_DRAW
    );
    glVertexAttribIPointer(attribute_index, coordinate_size, GL_INT,
        coordinate_size * sizeof(int), 0
    );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void cubemap_face(char *file_path, unsigned int face) {
    Image *data = image_load(file_path);
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA,
        data->width, data->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data->data
    );
    image_free(data);
}

void init_sky_box(
    Renderer *renderer,
    char *xp, char *xn, char *yp, char *yn, char *zp, char *zn
) {
    float vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &renderer->skybox.vao);
    glBindVertexArray(renderer->skybox.vao);

    store_float_in_attributes(
        &renderer->skybox.vbo, 0, 3, sizeof(vertices), vertices);
    renderer->skybox.vertex_count = sizeof(vertices)/sizeof(vertices[0]);

    glGenTextures(1, &renderer->skybox.texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, renderer->skybox.texture_id);

    cubemap_face(xp, 0);
    cubemap_face(xn, 1);
    cubemap_face(yp, 2);
    cubemap_face(yn, 3);
    cubemap_face(zp, 4);
    cubemap_face(zn, 5);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

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
    float y_scale = (float) ((1.0f / tan(rh->FOV*0.5)) * aspectRatio);
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
    shader_push(rh->anim_shader);
    shader_load_matrix(
        rh->anim_shader,
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
    rh->FOV = RENDERER_FOV;
	rh->NEAR_PLANE = RENDERER_NEAR_PLANE;
	rh->FAR_PLANE = RENDERER_FAR_PLANE;

	rh->RED = RENDERER_RED;
	rh->GREEN = RENDERER_GREEN;
	rh->BLUE = RENDERER_BLUE;

    rh->projection_matrix = create_projection_matrix(ctx, rh);

    rh->shader = shader_get_program();
    rh->anim_shader = shader_get_program_general(
        "shaders/anim_vertex.glsl", "shaders/fragment_shader.glsl"
    );
    rh->circle_shader = shader_get_program_general(
        "shaders/circle_vertex.glsl", "shaders/circle_fragment.glsl"
    );
    rh->gui_shader = shader_get_program_general(
        "shaders/gui_vertex.glsl", "shaders/gui_fragment.glsl"
    );
    log_if_err("There was an issue on renderer init\n");
    rh->sky_shader = shader_get_program_general(
        "shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl"
    );
    log_if_err("There was an issue wit skybox shader\n");

    shader_push(rh->shader);
    shader_load_matrix(
        rh->shader, "projection_matrix", &rh->projection_matrix
    );
    shader_push(rh->anim_shader);
    shader_load_matrix(
        rh->anim_shader, "projection_matrix", &rh->projection_matrix
    );
    shader_push(rh->circle_shader);
    shader_load_matrix(
        rh->circle_shader, "projection_matrix", &rh->projection_matrix
    );
    shader_push(rh->sky_shader);
    shader_load_matrix(
        rh->sky_shader, "projection_matrix", &rh->projection_matrix
    );
    shader_pop();

    RenderLayer *layers = calloc(1, sizeof(RenderLayer));
    layers->entities = NEW_LIST(Entity);
    layers->gui_entities = NEW_LIST(Entity);
    layers->font_entities = NEW_LIST(Entity);

    rh->layers = layers;

    rh->entities = NEW_LIST(Entity);
    rh->gui_entities = NEW_LIST(Entity);
    rh->font_entities = NEW_LIST(Entity);
    rh->debug_entities = NEW_LIST(Entity);


    init_sky_box(rh,
        "assets/textures/right.jpg", "assets/textures/left.jpg",
        "assets/textures/top.jpg", "assets/textures/bottom.jpg",
        "assets/textures/front.jpg", "assets/textures/back.jpg"
    );
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

void render_entities(Renderer *rh, List(Entity) *entities) {
    Vec3 light_color = rh->light->color;
    Entity entity;
    FOR_ALL(entity, entities) {
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
            entity.position, entity.rotation, entity.scale
        );
        shader_load_matrix(
            rh->shader, "transformation_matrix", &transformation_matrix
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
            log_if_err("Issue before restoring light\n");
            rh->light->color = light_color;
            shader_load_light(rh->shader, rh->light);
            log_if_err("There was a problem restoring lights");
        }
	}
}


void render_font_entities(Renderer *rh, List(Entity) *entities) {
    Entity entity;
    FOR_ALL(entity, entities) {

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
            entity.position, entity.rotation, entity.scale
        );
        shader_load_matrix(
            rh->gui_shader, "transformation_matrix", &transformation_matrix
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


void render_gui_entities(Renderer *rh, List(Entity) *entities) {
    Entity entity;
    FOR_ALL(entity, entities) {
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
            entity.position, entity.rotation, entity.scale
        );

        shader_load_matrix(
            rh->gui_shader, "transformation_matrix", &transformation_matrix
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

    glDepthMask(GL_FALSE);
    printf("Sky shader: %u\n", rh->sky_shader);
    printf("Skybox vao : %u\n", rh->skybox.vao);
    printf("Skybox texture : %u\n", rh->skybox.texture_id);
    printf("Skybox vertices : %u\n", rh->skybox.vertex_count);
    shader_push(rh->sky_shader);


    Mat4 skybox_mat = view_matrix;
    skybox_mat.m30 = 0.0;
    skybox_mat.m31 = 0.0;
    skybox_mat.m32 = 0.0;
    skybox_mat.m03 = 0.0;
    skybox_mat.m13 = 0.0;
    skybox_mat.m23 = 0.0;

    shader_load_matrix(rh->sky_shader, "view_matrix", &skybox_mat);

    glBindVertexArray(rh->skybox.vao);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, rh->skybox.texture_id);
    glDrawArrays(GL_TRIANGLES, 0, rh->skybox.vertex_count);
    glDepthMask(GL_TRUE);
    log_if_err("Renderer found a problem with skybox shader\n");

    shader_push(rh->anim_shader);
    shader_load_matrix(
        rh->anim_shader,
        "view_matrix",
        &view_matrix
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
    shader_load_light(rh->anim_shader, rh->light);
    log_if_err("Renderer found problem loading lights");

    render_entities(rh, rh->entities);
    glDisable(GL_DEPTH_TEST);

    shader_push(rh->gui_shader);

    render_gui_entities(rh, rh->gui_entities);
    render_font_entities(rh, rh->font_entities);

    for(RenderLayer *layer=rh->layers; layer; layer=layer->next) {
        shader_push(rh->shader);
        render_entities(rh, layer->entities);
        glDisable(GL_DEPTH_TEST);

        shader_push(rh->gui_shader);

        render_gui_entities(rh, layer->gui_entities);
        render_font_entities(rh, layer->font_entities);
    }


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


void init_floor_model(BaseModel *world_model) {
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    int mesh_size = 128 * 128;

    mesh->vertices = (Vec3 *)malloc(mesh_size * sizeof(Vec3));
    mesh->uvs = (Vec2 *)malloc(mesh_size * sizeof(Vec2));
    mesh->normal = (Vec3 *)malloc(mesh_size * sizeof(Vec3));
    mesh->indices = (unsigned int*)malloc(mesh_size * sizeof(unsigned int));
    mesh_init(mesh);

    load_data_to_model(
        world_model, (float *) mesh->vertices, mesh->indices,
        3*sizeof(float)*mesh->vertices_len,
        sizeof(unsigned int)*mesh->indices_len
    );
    load_texture_to_model(
        world_model,
        // "assets/textures/marble-floor.jpg",
        "assets/textures/wall.jpg",
        // "assets/textures/wood-floor.jpg",
        (float *)mesh->uvs, 
        2 * sizeof(float) * mesh->uvs_len
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(world_model->vao);
    store_float_in_attributes(
        &world_model->normal,
        2,
        3,
        3 * mesh->vertices_len * sizeof(float),
        (float *) mesh->normal
    );
    log_if_err("Issue after loading normals\n");

    world_model->vertex_count = mesh->indices_len;

    free(mesh->vertices);
    free(mesh->uvs);
    free(mesh->normal);
    free(mesh->indices);
    free(mesh);
}


void init_font(GraphicsContext *ctx, Renderer *renderer, Font *font) {
    log_if_err("Issue before Font initiation\n");
    font_init(
        font, "assets/fonts/VictorMono-Regular.ttf",
        (float)ctx->width, (float)ctx->height
    );
    renderer->font = font;
    log_if_err("Issue with Font initiation\n");
}


void load_model_from_obj(
    BaseModel *model, char *obj_file, char *texture_file
) {
    IntermediateModel tmp = {0};
    parse_obj_file(obj_file, &tmp);
    load_data_to_model(
        model, tmp.vertices, tmp.indices,
        tmp.vertices_count* sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        model, texture_file, tmp.uvs, tmp.uvs_count * sizeof(float)
    );
    glBindVertexArray(model->vao);
    store_float_in_attributes(
        &model->normal, 2, 3, tmp.normals_count * sizeof(float), tmp.normals
    );
    log_if_err("Issue after loading normals\n");
    model->vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);
}

void load_model_from_gltf(
    BaseModel *model, char *gltf_file, char *bin_file, char *texture_file
) {
    char *data = read_file(gltf_file);
    GltfData gltf = parse_gltf_data(data);
    char *binary = read_file(bin_file);

    IntermediateModel tmp = load_data_from_gltf(&gltf, binary);
    free(data);
    free(binary);
    
    load_data_to_model(
        model, tmp.vertices, tmp.indices,
        tmp.vertices_count * sizeof(float),
        tmp.indices_count * sizeof(unsigned int)
    );
    load_texture_to_model(
        model, texture_file, tmp.uvs, tmp.uvs_count * sizeof(float)
    );
    log_if_err("Issue before loading normals\n");
    glBindVertexArray(model->vao);
    store_float_in_attributes(
        &model->normal, 2, 3, tmp.normals_count * sizeof(float), tmp.normals
    );
    log_if_err("Issue after loading normals\n");
    model->vertex_count = tmp.indices_count;
    intermediate_model_free(&tmp);
}

