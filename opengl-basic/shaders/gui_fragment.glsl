#version 330 core


out vec4 FragColor;

in vec2 out_text_coords;
in vec4 out_color;

uniform sampler2D in_texture;


void main() {
    float texture_color = texture(in_texture, out_text_coords).r;
    // FragColor = mix(out_color, vec4(texture_color), out_color.a);
    FragColor = out_color * vec4(texture_color);
}
