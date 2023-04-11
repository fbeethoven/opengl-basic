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


void init_render_handler(GraphicsContext *ctx, Renderer *rh) {
    rh->FOV = 1.19377;
	rh->NEAR_PLANE = 0.1f;
	rh->FAR_PLANE = 1000;

	rh->RED = 0.4f;
	rh->GREEN = 0.4f;
	rh->BLUE = 0.4f;

    rh->projection_matrix = create_projection_matrix(ctx, rh);

    rh->shader = shader_get_program();
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
    shader_pop();
}


void prepare(Renderer *rh) {
    log_if_err("There was an issue BEFORE Preparing\n");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(rh->RED, rh->GREEN, rh->BLUE, 1);
    log_if_err("There was an issue Preparing\n");
}


void render(Renderer *rh, Camera *camera) {
    prepare(rh);
    shader_push(rh->shader);

    Mat4 view_matrix = mat4_look_at(
        camera->position, 
        camera->centre,
        newVec3(0.0, 1.0, 0.0)
    );

    shader_load_matrix(
        rh->shader,
        "view_matrix",
        &view_matrix
    );
    log_if_err("Issue before loading light\n");
    shader_load_light(rh->shader, rh->light);
    log_if_err("There was a problem loading lights");


    for (int i=0; i<10; i++) {
        Entity entity = rh->entities[i];

        if (entity.active == 0) {
            continue;
        }
        printf("Entity %d Debug INFO:\n", i);
        printf(
            "  position: %f %f %f\n",
            entity.position->x, entity.position->y, entity.position->z
        );
        printf(
            "  vao: %u, vbo: %u, ibo: %u\n",
            entity.model->vao,
            entity.model->vbo,
            entity.model->ibo
        );

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
            entity.scale
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
	}

    glDisableVertexAttribArray(2);
    glDisable(GL_DEPTH_TEST);

    shader_push(rh->gui_shader);

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
            entity.scale
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
        log_if_err("Issue Polygons\n");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rh->font->texture_id);
        log_if_err("Issue before drawing gui entities\n");

        glDrawElements(
            GL_TRIANGLES, rh->font->font_mesh->indices_len,
            GL_UNSIGNED_INT, 0
        );
        log_if_err("Issue after gui entities\n");
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

