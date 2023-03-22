#include "shader.h"


static char *read_file(char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Failed to read input\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    char *data = malloc(sizeof(char) * file_size);
    if(fread(data, sizeof(char), file_size, file) != file_size) {
        fprintf(stderr, "There was an error reading input");
        free(data);
        fclose(file);
        exit(1);
    }

    fclose(file);
    return data;
}


static unsigned int compile_shaders(GLenum shader_type, const char *shader_source) {
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


unsigned int shader_get_program() {
	unsigned int shader_program_id = glCreateProgram();

    char* vertex_shader = read_file("shaders/vertex_shader.glsl");
	unsigned int vertex_shader_id = compile_shaders(GL_VERTEX_SHADER, vertex_shader);
	glAttachShader(shader_program_id, vertex_shader_id);
	free(vertex_shader);

    char* fragment_shader = read_file("shaders/fragment_shader.glsl");
	unsigned int fragment_shader_id = compile_shaders(GL_FRAGMENT_SHADER, fragment_shader);
	glAttachShader(shader_program_id, fragment_shader_id);
	free(fragment_shader);

	glLinkProgram(shader_program_id);

	glDetachShader(shader_program_id, vertex_shader_id);
	glDetachShader(shader_program_id, fragment_shader_id);
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

    return shader_program_id;
}
