#version 460

out vec4 FragColor;

uniform sampler2D texture0;
uniform float weight[5];

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(texture0, pix, 0) * weight[0];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(1, 0)) * weight[1];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(-1, 0)) * weight[1];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(2, 0)) * weight[2];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(-2, 0)) * weight[2];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(3, 0)) * weight[3];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(-3, 0)) * weight[3];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(4, 0)) * weight[4];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(-4, 0)) * weight[4];

    FragColor = sum;
}
