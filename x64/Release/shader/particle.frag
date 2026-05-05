#version 460

in vec3  vColor;
in float vAlpha;
out vec4 fragColor;

void main()
{
    fragColor = vec4(vColor, vAlpha);
}
