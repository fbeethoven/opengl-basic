#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 text_coords;

out vec2 out_text_coords;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main() {
    gl_Position = transformation_matrix * vec4(position, 1.0);
    out_text_coords = text_coords;
}
