#version 300 es
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextCoords;

out vec2 TextCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    TextCoords = aTextCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
