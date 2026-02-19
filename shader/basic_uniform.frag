#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
layout(location = 0) out vec4 FragColor;

struct Light {
    vec3 Direction;
    vec3 La;
    vec3 Ld;
};

struct Material {
    vec3 Ka;
    vec3 Ks;
    float Shininess;
};

uniform Light light;
uniform Material material;
uniform sampler2D tex;

vec3 blinnPhongModel(vec3 position, vec3 normal, vec3 texColor)
{
    vec3 nnorm = normalize(normal);
    vec3 s = normalize(light.Direction);
    vec3 v = normalize(-position);
    vec3 h = normalize(v + s);

    // Ambient component
    vec3 ambient = material.Ka * light.La;

    // Diffuse component
    vec3 diffuse = texColor * light.Ld * max(0.0, dot(nnorm, s));

    // Specular component
    vec3 specular = material.Ks * light.Ld * pow(max(0.0, dot(nnorm, h)), material.Shininess);

    return ambient + diffuse + specular;
}

void main()
{
    vec3 texColor = texture(tex, TexCoord).rgb;
    vec3 lightColor = blinnPhongModel(Position, Normal, texColor);
    FragColor = vec4(lightColor, 1.0);
}
