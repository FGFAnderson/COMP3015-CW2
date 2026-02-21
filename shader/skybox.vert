#version 460

layout(location = 0) in vec3 VertexPosition;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = VertexPosition;
    vec4 pos = projection * view * vec4(VertexPosition, 1.0);
    // For depth testing optimisation, found in: https://learnopengl.com/Advanced-OpenGL/Cubemaps
    gl_Position = pos.xyww;
}
