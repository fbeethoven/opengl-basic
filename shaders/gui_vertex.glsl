 #version 330 core

in vec4 position;
in vec2 texCoord0;

out vec2 uv0;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;


void main() {
    vec4 world_position = transformation_matrix * vec4(position,1.0);
    gl_Position = projection_matrix * view_matrix * world_position;
    uv0 = texCoord0;
}
