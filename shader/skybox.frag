#version 460

in vec3 texCoords;
layout(location = 0) out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
    fragColor = texture(skybox, texCoords);
}
