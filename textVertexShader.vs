#version 300 es

layout(location = 0) in vec4 coord;
out vec2 texcoord;

uniform mat4 projection;

void main(void) {
    gl_Position = projection * vec4(coord.xy, 0.0, 1.0);
    texcoord = coord.zw;
}
