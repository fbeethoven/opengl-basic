#version 330 core


layout (location = 0) in vec3 aPos;

out float light;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
    light = clamp((1.0/aPos.z), 0.0, 1.0);
};

