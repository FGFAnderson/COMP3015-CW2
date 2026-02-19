#version 460

in vec3 Position;
in vec3 Normal;
layout(location = 0) out vec4 FragColor;

uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ld;
uniform vec4 LightPosition;
uniform float Shininess;

vec3 phongModel(vec3 position, vec3 normal)
{
    vec3 nnorm = normalize(normal);
    vec3 s = normalize(vec3(LightPosition) - position);
    vec3 v = normalize(-position);
    vec3 r = reflect(-s, nnorm);

    // Ambient component
    vec3 ambient = 0.2 * Kd * Ld;

    // Diffuse component
    vec3 diffuse = Kd * Ld * max(0.0, dot(nnorm, s));

    // Specular component
    vec3 specular = Ks * Ld * pow(max(0.0, dot(r, v)), Shininess);

    return ambient + diffuse + specular;
}

void main()
{
    FragColor = vec4(phongModel(Position, Normal), 1.0);
}
