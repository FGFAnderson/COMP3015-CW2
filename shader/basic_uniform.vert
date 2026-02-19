#version 460

// ./helper/trianglemesh.cpp defines VAO locations
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

void main()
{
    // Convert to eye space
    Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    Normal = normalize(NormalMatrix * VertexNormal);
    TexCoord = VertexTexCoord;

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
