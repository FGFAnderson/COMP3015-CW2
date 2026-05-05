#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "helper/glslprogram.h"

class ParticleSystem {
public:
    void init();
    void spawnBurst(glm::vec3 pos, glm::vec3 color, int count = 60);
    void update(float dt);
    void render(const glm::mat4& view, const glm::mat4& proj);

private:
    struct Particle {
        glm::vec4 initVelBirth;
        glm::vec4 spawnPosLife;
        glm::vec3 color;
        float _pad = 0.0f; // Fixxes memory alignment
    };

    std::vector<Particle> particles;

    GLSLProgram prog;
    GLuint vao = 0;
    GLuint boxVbo = 0, boxEbo = 0;
    GLuint instanceVbo = 0;
    int indexCount = 0;
    float simTime = 0.0f;

    static constexpr int MAX_PARTICLES = 512;

    void buildMesh();
};
