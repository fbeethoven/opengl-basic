#version 330 core

out vec4 FragColor;
in float light;
in vec4 color;

void main(){
    FragColor = color;
}
