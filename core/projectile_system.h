#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <deque>
#include <vector>

#include "helper/glslprogram.h"
#include "core/particle_system.h"

class PhysicsWorld;

class ProjectileSystem {
public:
    struct Enemy {
        glm::vec3 pos;
        glm::vec3 vel;
        float radius = 1.5f;
        int health = 3;
        bool alive = true;
        float shootTimer = 0.0f;
        float flashTimer = 0.0f;
    };

    ProjectileSystem() = default;

    void init();

    void addEnemy(glm::vec3 pos, float radius = 1.5f, int health = 3);
    void shoot(glm::vec3 origin, glm::vec3 direction, float speed = 300.0f);
    void update(float dt, glm::vec3 playerPos, PhysicsWorld& physics);

    void renderProjectiles(const glm::mat4& view, const glm::mat4& proj);
    void renderParticles(const glm::mat4& view, const glm::mat4& proj);

    struct PointLight { glm::vec3 pos; glm::vec3 color; };
    const std::vector<PointLight>& getPointLights(const glm::mat4& view);

    const std::vector<Enemy>& getEnemies() const { return enemies; }
    int  enemiesAlive() const;
    bool checkPlayerHit(glm::vec3 playerPos, float playerRadius = 0.5f);
    bool isEnemyFlashing(const Enemy& e) const { return e.flashTimer > 0.0f; }

    const std::deque<glm::vec3>& getRecentImpacts() const { return recentImpacts; }

private:
    struct Projectile {
        glm::vec3 pos;
        glm::vec3 prevPos;
        glm::vec3 vel;
        float life = 3.0f;
        static constexpr float RADIUS = 0.15f;
    };

    std::vector<Projectile> projectiles;
    std::vector<Projectile> enemyProjectiles;
    std::vector<Enemy>      enemies;
    std::deque<glm::vec3>   recentImpacts;
    std::vector<PointLight> lightCache;

    ParticleSystem particleSystem;

    GLSLProgram prog;
    GLuint vao = 0, vbo = 0, ebo = 0;
    int    indexCount = 0;

    static constexpr float ENEMY_PROJECTILE_SPEED = 200.0f;

    void buildBoxMesh();
    void recordImpact(glm::vec3 pos);
};
