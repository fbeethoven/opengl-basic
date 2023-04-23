#version 330 core


out vec4 FragColor;

in vec2 out_text_coords;
in vec3 out_color;

uniform sampler2D in_texture;

void main() {
    float value = texture(in_texture, out_text_coords).r;
    FragColor = vec4(out_color, value);
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
