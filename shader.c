#include "shader.h"
#include "utils/file_handler.h"




static unsigned int compile_shaders(
    GLenum shader_type, const char *shader_source
) {
	int status_success;

	unsigned int shader_id;
	shader_id = glCreateShader(shader_type);

	glShaderSource(shader_id, 1, &shader_source, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status_success);
	char infoLog[512];

	if(!status_success){
		glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPILATION::FAILED\n");
        printf("  %s\n", infoLog);
	}

    return shader_id;
}



unsigned int shader_get_program_general(
    char *vertex_shader_path, char* fragment_shader_path
) {
	unsigned int shader_program_id = glCreateProgram();

    char* vertex_shader = read_file(vertex_shader_path);
	unsigned int vertex_shader_id = compile_shaders(
        GL_VERTEX_SHADER, vertex_shader
    );
	glAttachShader(shader_program_id, vertex_shader_id);
	free(vertex_shader);

    char* fragment_shader = read_file(fragment_shader_path);
	unsigned int fragment_shader_id = compile_shaders(
        GL_FRAGMENT_SHADER, fragment_shader
    );
	glAttachShader(shader_program_id, fragment_shader_id);
	free(fragment_shader);

	glLinkProgram(shader_program_id);

	glDetachShader(shader_program_id, vertex_shader_id);
	glDetachShader(shader_program_id, fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

    return shader_program_id;
}


unsigned int shader_get_program_2d() {
    return shader_get_program_general(
        "shaders/gui_vertex.glsl",
        "shaders/gui_fragment.glsl"
    );
}


unsigned int shader_get_program() {
    return shader_get_program_general(
        "shaders/vertex_shader.glsl",
        "shaders/fragment_shader.glsl"
    );
}


void shader_load_vec2(
    unsigned int shader_program_id,
    char *location,
    Vec2 *vec
) {
    int uniform_location = glGetUniformLocation(
        shader_program_id, location
    );
    if (uniform_location < 0) {
        printf("VECTOR %s\n", location);
        printf("%f %f\n", vec->x, vec->y);
        printf(
            "[ERROR] shader (ID: %d) couldn't find location %s\n",
            shader_program_id,
            location
        );
    }
    glUniform2fv(uniform_location, GL_FALSE, (float *)vec);
}

void shader_load_float(
    unsigned int shader_program_id,
    char *location,
    float x
) {
    int uniform_location = glGetUniformLocation(
        shader_program_id, location
    );
    if (uniform_location < 0) {
        printf("FLOAT %s\n", location);
        printf("%f\n", x);
        printf(
            "[ERROR] shader (ID: %d) couldn't find location %s\n",
            shader_program_id,
            location
        );
    }
    glUniform1f(uniform_location, x);
}


void shader_load_vec3(
    unsigned int shader_program_id,
    char *location,
    Vec3 *vec
) {
    int uniform_location = glGetUniformLocation(
        shader_program_id, location
    );
    if (uniform_location < 0) {
        printf("VECTOR %s\n", location);
        printf("%f %f %f\n", vec->x, vec->y, vec->z);
        printf(
            "[ERROR] shader (ID: %d) couldn't find location %s\n",
            shader_program_id,
            location
        );
    }
    glUniform3fv(uniform_location, 1, (float *)vec);
}


void shader_load_vec4(
    unsigned int shader_program_id,
    char *location,
    Vec4 *vec
) {
    int uniform_location = glGetUniformLocation(
        shader_program_id, location
    );
    if (uniform_location < 0) {
        printf("VECTOR %s\n", location);
        printf("%f %f %f %f\n", vec->x, vec->y, vec->z, vec->z);
        printf(
            "[ERROR] shader (ID: %d) couldn't find location %s\n",
            shader_program_id,
            location
        );
    }
    glUniform4fv(uniform_location, 1, (float *)vec);
}

void shader_load_matrix(
    unsigned int shader_program_id,
    char *location,
    Mat4 *matrix
) {
    int uniform_location = glGetUniformLocation(
        shader_program_id, location
    );
    if (uniform_location < 0) {
        printf("MATRIX %s\n", location);

        printf("%f %f %f %f\n", matrix->m00, matrix->m01, matrix->m02, matrix->m03);
        printf("%f %f %f %f\n", matrix->m10, matrix->m11, matrix->m12, matrix->m13);
        printf("%f %f %f %f\n", matrix->m20, matrix->m21, matrix->m22, matrix->m23);
        printf("%f %f %f %f\n", matrix->m30, matrix->m31, matrix->m32, matrix->m33);


        printf(
            "[ERROR] shader (ID: %d) couldn't find location %s\n",
            shader_program_id,
            location
        );
    }
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, (float *)matrix);
}


void shader_push(unsigned int shader_program_id) {
    printf("Active shader: %d\n", shader_program_id);
    glUseProgram(shader_program_id);
}


void shader_pop() {
    glUseProgram(0);
}

