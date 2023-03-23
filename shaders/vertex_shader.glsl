#version 330 core


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out float light;
out vec4 color;
out vec2 TexCoor;

uniform mat4 u_MVP;
uniform mat4 u_transform;
uniform vec4 u_color= vec4(0.0f, 0.5f, 1.0f, 1.0f);

void main() {
    gl_Position = u_MVP * u_transform * vec4(aPos, 1.0);
    light = clamp((1.0/aPos.z), 0.0, 1.0);
    color = u_color;
    TexCoor = aTex;
}
