#version 460

out vec4 FragColor;

uniform sampler2D texture0;
uniform float weight[5];

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(texture0, pix, 0) * weight[0];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, 1)) * weight[1];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, -1)) * weight[1];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, 2)) * weight[2];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, -2)) * weight[2];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, 3)) * weight[3];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, -3)) * weight[3];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, 4)) * weight[4];
    sum += texelFetchOffset(texture0, pix, 0, ivec2(0, -4)) * weight[4];

    FragColor = sum;
}
