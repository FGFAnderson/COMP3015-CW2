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

// Lecture 3: https://dle.plymouth.ac.uk/pluginfile.php/3763254/mod_resource/content/1/Lecture3%20-%20Lighting%20and%20shading.pdf
// https://learnopengl.com/Lighting/Light-casters
struct Spotlight {
    vec3 Position; // Position in cam coords
    vec3 L; // Diffuse/spec intensity
    vec3 Direction; // Direction of the spotlight in cam coords.
    float Exponent; // Angular attenuation exponent
    float Cutoff; // Cutoff angle (between 0 and pi/2)
};

uniform Light light;
uniform Material material;
uniform sampler2D tex;
uniform Spotlight spotlight;

vec3 blinnPhongModel(vec3 position, vec3 normal, vec3 texColor)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.Direction);
    vec3 viewDir = normalize(-position);
    vec3 halfDir = normalize(viewDir + lightDir);

    // Ambient component
    vec3 ambient = material.Ka * light.La;

    // Diffuse component
    vec3 diffuse = texColor * light.Ld * max(0.0, dot(norm, lightDir));

    // Specular component
    vec3 specular = material.Ks * light.Ld * pow(max(0.0, dot(norm, halfDir)), material.Shininess);

    return ambient + diffuse + specular;
}

vec3 spotBlinnPhong(vec3 position, vec3 normal, vec3 texColor) {
    vec3 norm = normalize(normal);

    vec3 spotDir = normalize(spotlight.Position - position);
    float cosAng = dot(-spotDir, spotlight.Direction);
    float angle = acos(cosAng);
    float spotScale = 0.0;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (angle < spotlight.Cutoff) {
        spotScale = pow(cosAng, spotlight.Exponent);
        float dotProduct = dot(spotDir, norm);
        diffuse = texColor * max(0.0, dotProduct);

        if (dotProduct > 0.0) {
            vec3 viewDir = normalize(-position);
            vec3 halfDir = normalize(viewDir + spotDir);
            specular = material.Ks * pow(max(0.0, dot(norm, halfDir)), material.Shininess);
        }
    }

    return spotScale * spotlight.L * (diffuse + specular);
}

void main()
{
    vec3 texColor = texture(tex, TexCoord).rgb;
    vec3 directLight = blinnPhongModel(Position, Normal, texColor);
    vec3 spotLight = spotBlinnPhong(Position, Normal, texColor);
    vec3 lightColor = directLight + spotLight;
    FragColor = vec4(lightColor, 1.0);
}
