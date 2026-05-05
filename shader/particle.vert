#version 460

layout(location = 0) in vec3 meshPos;
layout(location = 1) in vec4 initVelBirth;
layout(location = 2) in vec4 spawnPosLife;
layout(location = 3) in vec3 color;

uniform mat4 view;
uniform mat4 projection;
uniform float currentTime;

out vec3 vColor;
out float vAlpha;

void main()
{
    vec3 initVel = initVelBirth.xyz;
    float birthTime = initVelBirth.w;
    vec3 spawnPos = spawnPosLife.xyz;
    float maxLife = spawnPosLife.w;

    float age = currentTime - birthTime;
    float t = age / maxLife;

    // P(t) = P0 + v0*t + 0.5*a*t^2 - https://developer.valvesoftware.com/wiki/Constant-Linear-Quadratic_Falloff
    vec3 gravity = vec3(0.0, -9.8, 0.0);
    vec3 worldPos = spawnPos + initVel * age + 0.5 * gravity * age * age + meshPos * 0.3;

    gl_Position = projection * view * vec4(worldPos, 1.0);
    vAlpha = (1.0 - t) * 0.8;
    vColor = color * (1.0 - t);
}
