#version 460

in vec2 FragTexCoord;
out vec4 fragColor;

uniform sampler2D screenTexture;

void main() {
    vec4 texColor = texture(screenTexture, FragTexCoord);
    texColor.a *= 0.4;
    fragColor = texColor;
}
