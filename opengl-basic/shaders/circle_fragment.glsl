#version 330 core


out vec4 FragColor;

in vec2 out_text_coords;
in vec3 out_color;

uniform sampler2D in_texture;

const float low = 0.9;
const float high = 1.0;

void main() {
    float value = texture(in_texture, out_text_coords).r;
    vec2 new_uv = vec2(2*out_text_coords.x - 1.0, 1.0 - 2*out_text_coords.y);
    float uv_len = length(new_uv);
    if ( low <= uv_len && uv_len <= high) {
        FragColor = vec4(out_color, value);
    }
    else {
        FragColor = vec4(out_color, 0.0);
    }
}
