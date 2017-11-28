#version 300 es
precision mediump float;
in vec3 Normal;
in vec3 FragPos;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 pos;
    vec4 color;
};

uniform Light light;

struct Material {
    sampler2D texture_diffuse1;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform Material material;
void main()
{
    // ambient
    vec4 ambient = light.color * material.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = light.color * diff * material.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec4 specular = light.color * spec * material.specular;

    float opacity = (ambient.a + diffuse.a + specular.a) / 3.0;
    vec3 result = ambient.rgb + diffuse.rgb + specular.rgb;
    fragColor = vec4(result, opacity);
}
