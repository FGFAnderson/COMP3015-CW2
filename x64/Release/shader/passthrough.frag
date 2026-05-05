#version 460

in vec2 texCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, texCoords);
}
