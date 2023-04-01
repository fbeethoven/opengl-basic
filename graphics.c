#include "graphics.h"
#include "shader.h"
#include "image.h"


void log_if_err(char *err_msg) {
    int err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("[ERROR: %d] GL Error: %s", err, err_msg);
        exit(1);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


// void graphics_free_object(GObject *object){
//     glDeleteVertexArrays(1, &object->vao);
//     glDeleteBuffers(1, &object->vbo);
//     glDeleteBuffers(1, &object->ibo);
//     glDeleteProgram(object->shader_program_id);
// }


void camera_move(Camera *camera, float dx, float dy, float dz) {
    camera->position.x += dx;
    camera->position.y += dy;
    camera->position.z += dz;
}

static void store_float_in_attributes(
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


static void bind_indices_buffer(
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

    // TODO: equivalent function for TextureModel
    // store_float_in_attributes(
    //     &new_object->vbo_texture, 1, 3, texture_size, texture
    // );

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


// void graphics_render_rect(GraphicsContext *ctx, GObject *object) {
//     glBindVertexArray(object->vao);
//     // glBindBuffer(GL_ARRAY_BUFFER, object->vbo);
//     glUseProgram(object->shader_program_id);
//     glDrawElements(GL_TRIANGLES, 2, GL_UNSIGNED_INT, 0);
//     glUseProgram(0);
//     glBindVertexArray(0);
// }

// GObject *graphics_new_rect(
//     GraphicsContext *ctx,
//     Vec3 *topright, Vec3 *topleft, Vec3 *botleft, Vec3 *botright
// ) {
//     glEnable(GL_DEPTH_TEST);
//     GObject *new_object = (GObject*) malloc(sizeof(GObject));
//     memset(new_object, 0, sizeof(GObject));
// 
// 
//     printf("RENDER DEBUG\n");
//     printf("x: %f, y: %f, z: %f\n", topright->x, topright->y, topright->z);
// 
//     float vertices[] = {
//         topright->x, topright->y, topright->z,
//         topleft->x, topleft->y, topleft->z,
//         botleft->x, botleft->y, botleft->z,
//         botright->x, botright->y, botright->z
//     };
// 
//     unsigned int indices[] = {
//         0, 1, 2,
//         2, 3, 0
//     };
// 
//     new_object->shader_program_id = shader_get_program_2d();
// 
//     // glCreateVertexArrays(1, &new_object->vao);
//     glGenVertexArrays(1, &new_object->vao);
//     glBindVertexArray(new_object->vao);
// 
//     glGenBuffers(1, &new_object->vbo);
// 	glBindBuffer(GL_ARRAY_BUFFER, new_object->vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// 
// 
//     glGenBuffers(1, &new_object->ibo);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_object->ibo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
// 
// 
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);
// 
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);
// 
//     return new_object;
// }


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

	rh->RED = 0.0f;
	rh->GREEN = 0.5f;
	rh->BLUE = 0.3f;

    rh->projection_matrix = create_projection_matrix(ctx, rh);
    // rh->projection_matrix.m33 = 1.0;

    rh->shader = shader_get_program();

    shader_push(rh->shader);
    shader_load_matrix(
        rh->shader,
        "projection_matrix",
        &rh->projection_matrix
    );
    shader_pop();
}


void prepare(Renderer *rh) {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(rh->RED, rh->GREEN, rh->BLUE, 1);
}


void render(Renderer *rh, Camera *camera) {
    // print_mat4("Projection Matrix:", &rh->projection_matrix);
    prepare(rh);
    shader_push(rh->shader);

    // Mat4 view_matrix = create_view_matrix(
    //     &camera->position, camera->pitch, camera->yaw
    // );
    
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

    for (int i=0; i<10; i++) {
        Entity entity = rh->entities[i];

        if (entity.active == 0) {
            // printf("Entity number %d: is not active\n", i);
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
        
        Mat4 transformation_matrix = create_transformation_matrix(
            entity.position,
            entity.rotation_x,
            entity.rotation_y,
            entity.rotation_z,
            entity.scale);
            shader_load_matrix(
                rh->shader,
                "transformation_matrix",
                &transformation_matrix
            );
        if ( (rh->fill & 1) == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entity.model->texture_id);
        glDrawElements(
            GL_TRIANGLES, entity.model->vertex_count, GL_UNSIGNED_INT, 0
        );
	}

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    shader_pop();
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

