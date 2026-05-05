#version 460

in vec3 position;
in vec2 texCoord;
in vec3 normal;
layout(location = 0) out vec4 fragColor;

struct Light {
    vec3 direction;
    vec3 la;
    vec3 ld;
};

struct Material {
    vec3 ka;
    vec3 ks;
    float shininess;
};

#define MAX_POINT_LIGHTS 64

uniform Light light;
uniform Material material;
uniform sampler2D tex;
uniform vec3 matColor;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightCol[MAX_POINT_LIGHTS];
uniform int numPointLights;

// https://learnopengl.com/Lighting/Basic-Lighting
vec3 blinnPhongModel(vec3 position, vec3 normal, vec3 texColor)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.direction);
    vec3 viewDir = normalize(-position);
    vec3 halfDir = normalize(viewDir + lightDir);

    vec3 ambient = material.ka * light.la * texColor;
    vec3 diffuse = texColor * light.ld * abs(dot(norm, lightDir));

    // Specular component
    vec3 specular = material.ks * light.ld * pow(max(0.0, dot(norm, halfDir)), material.shininess);

    return ambient + diffuse + specular;
}

vec3 pointLightsContrib(vec3 pos, vec3 norm, vec3 texColor)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < min(numPointLights, MAX_POINT_LIGHTS); i++) {
        vec3 toLight = pointLightPos[i] - pos;
        float dist = length(toLight);
        vec3 lightDir = toLight / dist;

        // Quadratic attenuation - https://developer.valvesoftware.com/wiki/Constant-Linear-Quadratic_Falloff
        float atten = 1.0 / (1.0 + 0.1 * dist + 0.15 * dist * dist);

        float diff = max(dot(norm, lightDir), 0.0);
        result += texColor * pointLightCol[i] * diff * atten;
    }
    return result;
}

void main()
{
    vec4 texSample = texture(tex, texCoord);
    vec3 texColor = texSample.rgb * matColor;
    float alpha = texSample.a;

    vec3 norm = normalize(normal);
    vec3 lightColor = blinnPhongModel(position, norm, texColor) +
            pointLightsContrib(position, norm, texColor);
    fragColor = vec4(lightColor, alpha);
}
