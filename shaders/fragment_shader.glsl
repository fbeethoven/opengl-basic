#version 330 core

out vec4 FragColor;
in float light;
in vec4 color;
in vec2 TexCoor;

uniform sampler2D ourTexture;

void main(){
    FragColor = texture(ourTexture, TexCoor);
}
