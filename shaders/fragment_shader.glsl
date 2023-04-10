#version 330 core

out vec4 FragColor;

in vec2 out_text_coords;
in vec3 out_light_color;
in vec3 to_light;
in vec3 surface_normal;

uniform sampler2D in_texture;


void main(){
    vec3 normal = normalize(surface_normal);
    vec3 normal_light = normalize(to_light);

    // float normal_dot = dot(surface_normal, to_light);
    // float brightness = max(normal_dot, 0.0);
    float normal_dot = dot(normal, normal_light);
    float brightness = max(normal_dot, 0.0);
    vec3 diffuse = brightness * out_light_color;


    vec4 c = texture(in_texture, out_text_coords);
    // FragColor = vec4(diffuse, 1.0) * c;
    FragColor = vec4(diffuse, 1.0);
    // FragColor = vec4(0.0, 1.0, 0.0, c.r);
}
