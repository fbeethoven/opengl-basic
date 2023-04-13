#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 text_coords;
layout (location = 2) in vec3 normal;

out vec2 out_text_coords;
out vec3 surface_normal;
out vec3 to_light;
out vec3 out_light_color;

uniform mat4 transformation_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_matrix;

uniform vec3 light_position;
uniform vec3 light_color;


void main() {
    vec4 world_position = transformation_matrix * vec4(position, 1.0);
    gl_Position = projection_matrix * view_matrix * world_position;

    out_text_coords = text_coords;

    vec3 surface_normal_abs = (transformation_matrix * vec4(normal, 1.0)).xyz;
    surface_normal = normalize(surface_normal_abs);

    // vec3 to_light_abs = light_position - world_position.xyz;
    to_light = normalize(-light_position);
    out_light_color = light_color;
}
