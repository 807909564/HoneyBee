#version 300 es
precision mediump float;
in vec2 TextCoords;
uniform sampler2D texture_diffuse1;
out vec4 fragColor;
void main()
{
    fragColor = texture(texture_diffuse1, TextCoords);
}
