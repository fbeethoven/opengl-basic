#version 330 core

out vec4 FragColor;

in vec2 out_text_coords;

uniform sampler2D in_texture;


void main(){
    FragColor = texture(in_texture, out_text_coords);
    // FragColor = vec4(0.0, 0.6, 0.8, 1.0);
}
