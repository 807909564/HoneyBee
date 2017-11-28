#version 300 es
precision highp float;

uniform sampler2D tex;
uniform vec4 color;

in vec2 texcoord;
out vec4 fragColor;

void main(void) {
    fragColor = vec4(1.0, 1.0, 1.0, texture2D(tex, texcoord).r) * color;
}
