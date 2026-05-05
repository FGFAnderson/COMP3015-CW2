#include "projectile_system.h"
#include "core/physics_world.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cmath>
#include <random>

static std::mt19937 rng{std::random_device{}()};

void ProjectileSystem::init() {
    prog.compileShader("shader/projectile.vert");
    prog.compileShader("shader/projectile.frag");
    prog.link();
    buildBoxMesh();
    particleSystem.init();
}

void ProjectileSystem::buildBoxMesh() {
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

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void ProjectileSystem::addEnemy(glm::vec3 pos, float radius, int health) {
    std::uniform_real_distribution<float> dis(0.0f, 1.5f);
    float randomShootDelay = dis(rng);
    enemies.push_back({pos, glm::vec3(0.0f), radius, health, true, randomShootDelay, 0.0f});
}

void ProjectileSystem::shoot(glm::vec3 origin, glm::vec3 direction, float speed) {
    Projectile p;
    p.pos = origin;
    p.prevPos = origin;
    p.vel = glm::normalize(direction) * speed;
    p.life = 4.0f;
    projectiles.push_back(p);
}

void ProjectileSystem::update(float dt, glm::vec3 playerPos, PhysicsWorld& physics) {
    for (auto& p : projectiles) {
        p.prevPos = p.pos;
        p.pos += p.vel * dt;
        p.life -= dt;

        if (!physics.inMapBounds(p.pos)) { p.life = 0.0f; continue; }

        if (physics.rayTest(p.prevPos, p.pos)) {
            recordImpact(p.pos);
            p.life = 0.0f;
            continue;
        }

        for (auto& e : enemies) {
            if (!e.alive) continue;
            if (glm::length(p.pos - e.pos) < e.radius + Projectile::RADIUS) {
                e.health--;
                e.flashTimer = 0.3f;
                p.life = 0.0f;
                recordImpact(p.pos);
                if (e.health <= 0) {
                    particleSystem.spawnBurst(e.pos, glm::vec3(1.0f, 0.3f, 0.1f));
                    e.alive = false;
                }
                break;
            }
        }
    }

    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
                       [](const Projectile& p) { return p.life <= 0.0f; }),
        projectiles.end());

    for (auto& e : enemies) {
        if (!e.alive) continue;

        e.flashTimer -= dt;

        glm::vec3 dir = playerPos - e.pos;
        float dist = glm::length(dir);
        if (dist > 0.01f) {
            e.vel = glm::normalize(dir) * 5.0f;
            e.pos += e.vel * dt;
        }

        e.shootTimer -= dt;
        if (e.shootTimer <= 0.0f) {
            if (dist > 0.01f) {
                Projectile p;
                p.pos = e.pos;
                p.prevPos = e.pos;
                p.vel = glm::normalize(dir) * ENEMY_PROJECTILE_SPEED;
                p.life = 5.0f;
                enemyProjectiles.push_back(p);
            }
            std::uniform_real_distribution<float> shootDis(1.0f, 2.5f);
            e.shootTimer = shootDis(rng);
        }
    }

    for (auto& p : enemyProjectiles) {
        p.prevPos = p.pos;
        p.pos += p.vel * dt;
        p.life -= dt;

        if (!physics.inMapBounds(p.pos)) { p.life = 0.0f; continue; }

        if (physics.rayTest(p.prevPos, p.pos))
            p.life = 0.0f;
    }

    enemyProjectiles.erase(
        std::remove_if(enemyProjectiles.begin(), enemyProjectiles.end(),
                       [](const Projectile& p) { return p.life <= 0.0f; }),
        enemyProjectiles.end());

    particleSystem.update(dt);
}

static glm::mat4 elongatedTransform(glm::vec3 pos, glm::vec3 dir, float width, float length) {
    glm::mat4 rot = glm::mat4_cast(glm::rotation(glm::vec3(0,0,1), glm::normalize(dir)));
    return glm::translate(glm::mat4(1.0f), pos)
         * rot
         * glm::scale(glm::mat4(1.0f), glm::vec3(width, width, length));
}

void ProjectileSystem::renderProjectiles(const glm::mat4& view, const glm::mat4& proj) {
    prog.use();
    glBindVertexArray(vao);

    auto drawBatch = [&](const std::vector<Projectile>& list, glm::vec3 color, float alpha, float width, float length) {
        prog.setUniform("color", color);
        prog.setUniform("alpha", alpha);
        for (const auto& pr : list) {
            glm::mat4 model = elongatedTransform(pr.pos, glm::normalize(pr.vel), width, length);
            prog.setUniform("mvp", proj * view * model);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        }
    };

    drawBatch(projectiles, glm::vec3(5.0f, 4.0f, 1.0f), 1.0f, 1.0f, 4.0f);
    drawBatch(enemyProjectiles, glm::vec3(4.0f, 0.8f, 0.5f), 0.8f, 0.8f, 3.0f);

    glBindVertexArray(0);
}

void ProjectileSystem::renderParticles(const glm::mat4& view, const glm::mat4& proj) {
    particleSystem.render(view, proj);
}

int ProjectileSystem::enemiesAlive() const {
    int count = 0;
    for (const auto& e : enemies) count += e.alive;
    return count;
}

bool ProjectileSystem::checkPlayerHit(glm::vec3 playerPos, float playerRadius) {
    for (auto& p : enemyProjectiles) {
        if (glm::length(p.pos - playerPos) < playerRadius + Projectile::RADIUS) {
            p.life = 0.0f;
            return true;
        }
    }
    return false;
}

const std::vector<ProjectileSystem::PointLight>& ProjectileSystem::getPointLights(const glm::mat4& view) {
    lightCache.clear();
    lightCache.reserve(projectiles.size() + enemyProjectiles.size());

    for (const auto& p : projectiles)
        lightCache.push_back({ glm::vec3(view * glm::vec4(p.pos, 1.0f)), glm::vec3(35.0f, 28.0f, 7.0f) });

    for (const auto& p : enemyProjectiles)
        lightCache.push_back({ glm::vec3(view * glm::vec4(p.pos, 1.0f)), glm::vec3(4.0f, 0.4f, 0.1f) });

    return lightCache;
}

void ProjectileSystem::recordImpact(glm::vec3 pos) {
    recentImpacts.push_back(pos);
    if (recentImpacts.size() > 20)
        recentImpacts.pop_front();
}
