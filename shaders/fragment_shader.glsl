#version 330 core

out vec4 FragColor;
in float light;

void main(){
    FragColor = light * vec4(1.0f, 0.5f, 0.2f, 1.0f);
}

