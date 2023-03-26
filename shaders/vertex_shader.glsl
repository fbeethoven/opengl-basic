#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 text_coords;

out vec2 out_text_coords;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main() {
    vec4 world_position = transformation_matrix * vec4(position,1.0);
    gl_Position = projection_matrix * view_matrix * world_position;
    out_text_coords = text_coords;
}
