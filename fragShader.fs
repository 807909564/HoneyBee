#version 300 es
precision mediump float;
in vec2 TextCoords;
out vec4 fragColor;

struct Material {
    sampler2D texture_diffuse1;
};
uniform Material material;
void main()
{
    fragColor = texture(material.texture_diffuse1, TextCoords);
}
