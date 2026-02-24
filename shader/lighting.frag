#version 460

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
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

// Lecture 3: https://dle.plymouth.ac.uk/pluginfile.php/3763254/mod_resource/content/1/Lecture3%20-%20Lighting%20and%20shading.pdf
// https://learnopengl.com/Lighting/Light-casters
struct Spotlight {
    vec3 position; // Position in cam coords
    vec3 l; // Diffuse/spec intensity
    vec3 direction; // direction of the spotlight in cam coords.
    float exponent; // Angular attenuation exponent
    float cutoff; // cutoff angle (between 0 and pi/2)
};

struct Fog {
    float maxDist;
    float minDist;
    vec3 color;
};

uniform Light light;
uniform Material material;
uniform sampler2D tex;
uniform sampler2D alphaTex;
uniform bool hasAlphaMap;
uniform Spotlight spotlight;
uniform Fog fog;
uniform sampler2D normalMap;

// Lecture 3: https://dle.plymouth.ac.uk/pluginfile.php/3763254/mod_resource/content/1/Lecture3%20-%20Lighting%20and%20shading.pdf
vec3 fogModel(vec3 position, vec3 color) {
    float distance = abs(position.z);

    float fogFactor = (fog.maxDist - distance) / (fog.maxDist - fog.minDist);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 fogColor = mix(fog.color, color, fogFactor);

    return fogColor;
}

// https://learnopengl.com/Lighting/Basic-Lighting
vec3 blinnPhongModel(vec3 position, vec3 normal, vec3 texColor)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.direction);
    vec3 viewDir = normalize(-position);
    vec3 halfDir = normalize(viewDir + lightDir);

    // Ambient component
    vec3 ambient = material.ka * light.la;

    // Diffuse component
    vec3 diffuse = texColor * light.ld * max(0.0, dot(norm, lightDir));

    // Specular component
    vec3 specular = material.ks * light.ld * pow(max(0.0, dot(norm, halfDir)), material.shininess);

    return ambient + diffuse + specular;
}

// https://learnopengl.com/Lighting/Light-casters
vec3 spotBlinnPhong(vec3 position, vec3 normal, vec3 texColor) {
    vec3 norm = normalize(normal);

    vec3 spotDir = normalize(spotlight.position - position);
    float cosAng = dot(-spotDir, spotlight.direction);
    float angle = acos(cosAng);
    float spotScale = 0.0;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    if (angle < spotlight.cutoff) {
        spotScale = pow(cosAng, spotlight.exponent);
        float dotProduct = dot(spotDir, norm);
        diffuse = texColor * max(0.0, dotProduct);

        if (dotProduct > 0.0) {
            vec3 viewDir = normalize(-position);
            vec3 halfDir = normalize(viewDir + spotDir);
            specular = material.ks * pow(max(0.0, dot(norm, halfDir)), material.shininess);
        }
    }

    return spotScale * spotlight.l * (diffuse + specular);
}

// AI USAGE, AI was used to summarise and explain questions on normal mapping. Refer to : ./ai_transcript/normal_map_explanation.md
vec3 sampleNormalMap(vec2 texCoord) {
    // Convert normal map to -1,1 a range
    vec3 sampledNormal = texture(normalMap, texCoord).rgb * 2.0 - 1.0;

    mat3 TBN = mat3(normalize(tangent), normalize(bitangent), normalize(normal));

    return normalize(TBN * sampledNormal);
}

void main()
{
    vec3 texColor = texture(tex, texCoord).rgb;
    float alpha = hasAlphaMap ? texture(alphaTex, texCoord).r : 1.0;
    vec3 normalDirection = sampleNormalMap(texCoord);

    // Handle back-facing surfaces
    if(!gl_FrontFacing)
        normalDirection = -normalDirection;

    vec3 directLight = blinnPhongModel(position, normalDirection, texColor);
    vec3 spotLight = spotBlinnPhong(position, normalDirection, texColor);
    vec3 lightColor = directLight + spotLight;
    vec3 fogColor = fogModel(position, lightColor);
    fragColor = vec4(fogColor, alpha);
}
