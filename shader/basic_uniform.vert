#version 460

// ./helper/trianglemesh.cpp defines VAO locations
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec3 position;
out vec3 normal;
out vec2 texCoord;

uniform mat4 modelViewMatrix;
uniform mat4 mvp;
uniform mat3 normalMatrix;

void main()
{
    // Convert to eye space
    position = vec3(modelViewMatrix * vec4(VertexPosition, 1.0));
    normal = normalize(normalMatrix * VertexNormal);
    texCoord = VertexTexCoord;

    gl_Position = mvp * vec4(VertexPosition, 1.0);
}
