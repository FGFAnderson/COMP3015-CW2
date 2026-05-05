#version 460

out vec2 texCoords;

void main()
{
    vec2 pos[4] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0));
    vec2 uv[4]  = vec2[](vec2(0.0, 0.0),  vec2(1.0, 0.0),  vec2(0.0, 1.0),  vec2(1.0, 1.0));
    texCoords = uv[gl_VertexID];
    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
}
