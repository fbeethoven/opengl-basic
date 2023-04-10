#version 330 core

out vec4 FragColor;

in vec2 out_text_coords;

uniform sampler2D in_texture;

const float width = 0.7;
const float edge = 0.1;

const float border_width = 0.8;
const float border_edge = 0.1;

const vec3 border_color = vec3(0.0); 

void main(){
    float distance = 1.0 - texture(in_texture, out_text_coords).r;
    float alpha = 1.0 - smoothstep(width, width + edge, distance);

    float border_distance = 1.0 - texture(in_texture, out_text_coords).r;
    float border_alpha = 1.0 - smoothstep(
        border_width, border_width + border_edge, border_distance
    );

    float final_alpha = alpha + (1.0 - alpha) * border_alpha;
    vec3 color = mix(border_color, vec3(1.0), alpha / final_alpha); 
    FragColor = vec4(color, final_alpha);
}
