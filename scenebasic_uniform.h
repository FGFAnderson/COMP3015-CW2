#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <glad/glad.h>
#include <vector>
#include "helper/glslprogram.h"
#include "core/mesh_instance.h"
#include <optional>
#include "core/physics_world.h"
#include "core/flycamera.h"
#include "core/projectile_system.h"
#include "core/skybox.h"
#include "core/crosshair.h"
#include "core/post_process.h"
#include "core/hud.h"
#include <GLFW/glfw3.h>

class SceneBasic_Uniform : public Scene {
private:
    // Scene
    GLSLProgram prog;
    std::vector<MeshInstance> meshInstances;

    // Hit effects
    float blurTimer = 0.0f;
    float shakeTimer = 0.0f;
    static constexpr float BLUR_DURATION = 0.6f;
    static constexpr float SHAKE_DURATION = 0.4f;

    PhysicsWorld physics;
    FlyCamera camera;
    ProjectileSystem projectiles;

    SkyboxRenderer skybox;
    Crosshair crosshair;
    PostProcessPipeline postProc;
    HUD hud;

    std::optional<MeshInstance> enemyMesh;
    bool prevLeftClick = false;

    float prevTime = 0.0f;

    static constexpr int MAX_HEALTH = 10;
    int playerHealth = MAX_HEALTH;
    int enemiesKilled = 0;
    bool gameOver = false;
    int prevEnemiesAlive = 0;
    int cachedEnemiesAlive = 0;

    float shootCooldown = 0.0f;
    static constexpr float SHOOT_COOLDOWN_MAX = 0.2f;

    // Spawning system
    int currentWave = 0;
    bool waveSpawned = false;
    float spawnTimer = 0.0f;
    static constexpr float SPAWN_DELAY = 2.0f;
    static constexpr float ENEMY_SPAWN_RADIUS = 40.0f;

    void compile();
    void renderScene();
    void spawnWave();

public:
    SceneBasic_Uniform();
    ~SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int w, int h);
    void processInput(GLFWwindow* window, float deltaTime);
    void processMouseMovement(double xpos, double ypos);
};

#endif // SCENEBASIC_UNIFORM_H
