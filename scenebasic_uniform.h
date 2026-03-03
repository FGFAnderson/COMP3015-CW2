#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <glad/glad.h>
#include <vector>
#include "helper/glslprogram.h"
#include "core/mesh_instance.h"
#include "core/skybox_instance.h"
#include "core/flycamera.h"

class SceneBasic_Uniform : public Scene
{
private:
    // Scene
    GLSLProgram prog;
    GLSLProgram skyboxProg;
    glm::vec3 lightDirection;
    std::vector<MeshInstance> meshInstances;
    SkyboxInstance skybox;
    FlyCamera camera;

    // HDR
    GLSLProgram toneMapProg;
    GLuint hdrFBO;
    GLuint hdrColorTexture;
    GLuint hdrDepthRBO;
    float logAverageLuminance;
    GLuint quadVAO;
    GLuint quadVBO;

    // Blur
    GLSLProgram blurVerticalProg;
    GLSLProgram blurHorizontalProg;
    GLuint blurFBO;
    GLuint blurTexture1;
    GLuint blurTexture2;

    void compile();
    void setupFBO();
    void setupQuad();
    void renderQuad();
    void computeLogAverageLuminance();
    void renderScene();
    void blurVertical();
    void blurHorizontal();
    void tonemap();
    float gauss(float x, float sigma2);

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void processInput(GLFWwindow* window, float deltaTime);
    void processMouseMovement(double xpos, double ypos);
};

#endif // SCENEBASIC_UNIFORM_H
