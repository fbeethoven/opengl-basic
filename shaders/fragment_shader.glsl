#version 330 core

out vec4 FragColor;

in vec2 out_text_coords;

uniform sampler2D in_texture;


void main(){
    vec4 c = texture(in_texture, out_text_coords);
    FragColor = c;
    // FragColor = vec4(0.0, 1.0, 0.0, c.r);
}
