#include "scenebasic_uniform.h"
#include <glad/glad.h>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/constants.hpp>
#include <random>
#include <cmath>
#include <cstdlib>
using glm::vec3;
using glm::mat4;

static constexpr float MAP_SCALE = 0.1f;

SceneBasic_Uniform::SceneBasic_Uniform()
    : camera(glm::vec3(0.0f, 2.0f, 0.0f), 8.0f, 0.1f) {
    Material mapMaterial;
    mapMaterial.ka = vec3(0.2f);
    mapMaterial.ks = vec3(0.1f);
    mapMaterial.shininess = 16.0f;

    meshInstances.emplace_back(
        "./media/models/map2.fbx",
        mapMaterial,
        glm::scale(mat4(1.0f), vec3(MAP_SCALE))
    );

    physics.loadMap("./media/models/map2.fbx", MAP_SCALE);
}

void SceneBasic_Uniform::compile() {
    try {
        prog.compileShader("shader/lighting.vert");
        prog.compileShader("shader/lighting.frag");
        prog.link();
    } catch (GLSLProgramException& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::spawnWave() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> angleDistrib(0.0f, glm::two_pi<float>());

    int enemyCount = 4 + currentWave;
    glm::vec3 playerPos = physics.getPlayerPosition();

    for (int i = 0; i < enemyCount; i++) {
        float angle = angleDistrib(gen);
        float x = playerPos.x + ENEMY_SPAWN_RADIUS * std::cos(angle);
        float z = playerPos.z + ENEMY_SPAWN_RADIUS * std::sin(angle);
        projectiles.addEnemy(glm::vec3(x, 1.0f, z), 3.5f);
    }
}

void SceneBasic_Uniform::initScene() {
    compile();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = glm::perspective(glm::radians(90.0f), float(width) / float(height), 0.1f, 500.0f);

    projectiles.init();

    prog.use();
    prog.setUniform("light.la", vec3(0.10f));
    prog.setUniform("light.ld", vec3(0.3f));
    prog.setUniform("matColor", glm::vec3(1.0f, 1.0f, 1.0f));

    Material enemyMat;
    enemyMat.ka = vec3(0.4f);
    enemyMat.ks = vec3(0.4f);
    enemyMat.shininess = 32.0f;
    enemyMesh.emplace("./media/models/ship.fbx", enemyMat, mat4(1.0f));

    crosshair.init(width, height, "./media/textures/Sight_64x64_008.png");
    skybox.init("./media/textures/skybox/space");
    postProc.init(width, height);
    prevTime = glfwGetTime();
}

void SceneBasic_Uniform::update(float t) {
    float dt = t - prevTime;
    prevTime = t;

    if (gameOver) return;

    shootCooldown -= dt;
    if (shootCooldown < 0.0f) shootCooldown = 0.0f;

    if (!waveSpawned) {
        spawnTimer -= dt;
        if (spawnTimer <= 0.0f) {
            spawnWave();
            waveSpawned = true;
        }
    } else {
        if (projectiles.enemiesAlive() == 0) {
            currentWave++;
            waveSpawned = false;
            spawnTimer = SPAWN_DELAY;
        }
    }

    physics.step(dt);
    glm::vec3 playerPos = physics.getPlayerPosition();
    camera.setPosition(playerPos);
    projectiles.update(dt, playerPos, physics);

    if (projectiles.checkPlayerHit(playerPos, 0.5f)) {
        playerHealth--;
        blurTimer = BLUR_DURATION;
        shakeTimer = SHAKE_DURATION;
        if (playerHealth <= 0) {
            gameOver = true;
            blurTimer = shakeTimer = 0.0f;
        }
    }

    blurTimer = std::max(0.0f, blurTimer - dt);
    shakeTimer = std::max(0.0f, shakeTimer - dt);

    cachedEnemiesAlive = projectiles.enemiesAlive();
    if (cachedEnemiesAlive < prevEnemiesAlive)
        enemiesKilled += (prevEnemiesAlive - cachedEnemiesAlive);
    prevEnemiesAlive = cachedEnemiesAlive;

    view = camera.getViewMatrix();

    // Camera shake
    if (shakeTimer > 0.0f) {
        static std::mt19937 shakeRng{std::random_device{}()};
        static std::uniform_real_distribution<float> shakeDist(-0.5f, 0.5f);
        float strength = shakeTimer / SHAKE_DURATION;
        float dx = shakeDist(shakeRng) * strength;
        float dy = shakeDist(shakeRng) * strength;
        view = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0.0f)) * view;
    }

    prog.use();
    prog.setUniform("light.direction",
        glm::normalize(vec3(view * glm::vec4(1.5f, -2.5f, 1.0f, 0.0f))));
}

void SceneBasic_Uniform::renderScene() {
    postProc.beginScene();
    prog.use();
    prog.setUniform("matColor", glm::vec3(1.0f, 1.0f, 1.0f));

    // Upload projectile point lights (eye-space positions + colours)
    const auto& lights = projectiles.getPointLights(view);
    int numLights = std::min((int)lights.size(), 64);
    prog.setUniform("numPointLights", numLights);
    char buf[32];
    for (int i = 0; i < numLights; i++) {
        snprintf(buf, sizeof(buf), "pointLightPos[%d]", i);
        prog.setUniform(buf, lights[i].pos);
        snprintf(buf, sizeof(buf), "pointLightCol[%d]", i);
        prog.setUniform(buf, lights[i].color);
    }

    for (auto& m : meshInstances)
        m.render(prog, view, projection);

    static constexpr float ENEMY_SCALE = 0.02f;
    for (const auto& e : projectiles.getEnemies()) {
        if (!e.alive) continue;

        if (glm::length(camera.getPosition() - e.pos) > 0.01f) {
            glm::mat4 rotMat(glm::transpose(glm::mat3(glm::lookAt(e.pos, camera.getPosition(), glm::vec3(0,1,0)))));
            enemyMesh->setTransform(glm::translate(mat4(1.0f), e.pos) * rotMat * glm::scale(mat4(1.0f), vec3(ENEMY_SCALE)));
        } else {
            enemyMesh->setTransform(glm::scale(glm::translate(mat4(1.0f), e.pos), vec3(ENEMY_SCALE)));
        }

        prog.setUniform("matColor", projectiles.isEnemyFlashing(e)
            ? glm::vec3(2.0f, 0.0f, 0.0f)
            : glm::vec3(1.6f, 1.6f, 1.6f));

        enemyMesh->render(prog, view, projection);
    }

    prog.setUniform("matColor", glm::vec3(1.0f, 1.0f, 1.0f));

    projectiles.renderProjectiles(view, projection);
    projectiles.renderParticles(view, projection);
    skybox.render(view, projection);
    crosshair.render();
}

void SceneBasic_Uniform::render() {
    renderScene();
    postProc.process(blurTimer > 0.0f);

    if (hud.isInitialized())
        hud.render(playerHealth, MAX_HEALTH, enemiesKilled, cachedEnemiesAlive, gameOver);
}

void SceneBasic_Uniform::resize(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
    projection = glm::perspective(glm::radians(90.0f), float(w) / float(h), 0.1f, 500.0f);
}

void SceneBasic_Uniform::processInput(GLFWwindow* window, float /*deltaTime*/) {
    if (!hud.isInitialized())
        hud.init(window);
    hud.newFrame();

    glm::vec3 fwd = camera.getForward();
    glm::vec3 flat = glm::normalize(glm::vec3(fwd.x, 0.0f, fwd.z));
    glm::vec3 right = glm::normalize(glm::cross(flat, glm::vec3(0.0f, 1.0f, 0.0f)));

    glm::vec3 dir(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) dir += flat;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) dir -= flat;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) dir -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) dir += right;

    if (glm::length(dir) > 0.001f) dir = glm::normalize(dir) * 0.4f;
    physics.setWalkDirection(glm::vec2(dir.x, dir.z));

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        physics.jump();

    bool leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (leftClick && !prevLeftClick && shootCooldown <= 0.0f) {
        projectiles.shoot(camera.getPosition() - glm::vec3(0.0f, 2.0f, 0.0f), camera.getForward(), 1000.0f);
        shootCooldown = SHOOT_COOLDOWN_MAX;
    }
    prevLeftClick = leftClick;
}

void SceneBasic_Uniform::processMouseMovement(double xpos, double ypos) {
    camera.processMouseMovement((float)xpos, (float)ypos);
}

SceneBasic_Uniform::~SceneBasic_Uniform() {
}
