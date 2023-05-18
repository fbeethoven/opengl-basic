#version 330 core

out vec4 FragColor;

in vec2 out_text_coords;
in vec3 out_light_color;
in vec3 to_light;
in vec3 surface_normal;

uniform sampler2D in_texture;

const float ambient = 0.2;

void main(){

    float normal_dot = dot(surface_normal, to_light);
    float brightness = max(normal_dot, 0.0);
    vec3 diffuse = (brightness + ambient) * out_light_color;

    vec4 c = texture(in_texture, out_text_coords);
    c = pow(c, vec4(2.2));
    vec4 color = vec4(diffuse, 1.0) * c;
    color = pow(color, vec4(1.0/2.2));
    FragColor = color;
}
