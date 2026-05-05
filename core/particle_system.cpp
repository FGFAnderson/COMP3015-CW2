#include "particle_system.h"
#include <glm/gtc/constants.hpp>
#include <random>
#include <cmath>
#include <algorithm>

static std::mt19937 particleRng{std::random_device{}()};

// Particle system inspired by Lecutre 7 and Lab 7

void ParticleSystem::init() {
    prog.compileShader("shader/particle.vert");
    prog.compileShader("shader/particle.frag");
    prog.link();
    buildMesh();
}

void ParticleSystem::buildMesh() {
    float h = 0.6f;
    float verts[] = {
        -h,-h,-h,  h,-h,-h,  h, h,-h, -h, h,-h,
        -h,-h, h,  h,-h, h,  h, h, h, -h, h, h,
    };
    GLuint idx[] = {
        0,1,2, 0,2,3,
        4,6,5, 4,7,6,
        0,4,5, 0,5,1,
        3,2,6, 3,6,7,
        0,3,7, 0,7,4,
        1,5,6, 1,6,2,
    };
    indexCount = 36;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &boxVbo);
    glBindBuffer(GL_ARRAY_BUFFER, boxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &boxEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)16);
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)32);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

void ParticleSystem::spawnBurst(glm::vec3 pos, glm::vec3 color, int count) {
    std::uniform_real_distribution<float> angleDist(0.0f, glm::two_pi<float>());
    std::uniform_real_distribution<float> speedDist(5.0f, 15.0f);

    for (int i = 0; i < count; i++) {
        float angle  = angleDist(particleRng);
        float speed  = speedDist(particleRng);
        float height = speedDist(particleRng) * 0.3f;
        glm::vec3 vel(std::cos(angle) * speed, height, std::sin(angle) * speed);

        Particle p;
        p.initVelBirth = glm::vec4(vel, simTime);
        p.spawnPosLife = glm::vec4(pos, 0.8f);
        p.color = color;
        particles.push_back(p);
    }
}

void ParticleSystem::update(float dt) {
    simTime += dt;
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [this](const Particle& p) {
                return simTime - p.initVelBirth.w >= p.spawnPosLife.w;
            }),
        particles.end());
}

void ParticleSystem::render(const glm::mat4& view, const glm::mat4& proj) {
    if (particles.empty()) return;

    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());

    prog.use();
    prog.setUniform("view", view);
    prog.setUniform("projection", proj);
    prog.setUniform("currentTime", simTime);

    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, (int)particles.size());

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(0);
}
