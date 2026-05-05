#version 460

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float weight[5];

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(screenTexture, pix, 0) * weight[0];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, 1)) * weight[1];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, -1)) * weight[1];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, 2)) * weight[2];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, -2)) * weight[2];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, 3)) * weight[3];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, -3)) * weight[3];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, 4)) * weight[4];
    sum += texelFetchOffset(screenTexture, pix, 0, ivec2(0, -4)) * weight[4];

    FragColor = sum;
}
