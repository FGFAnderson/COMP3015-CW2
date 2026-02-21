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
    GLSLProgram prog;
    GLSLProgram skyboxProg;
    glm::vec3 lightDirection;
    std::vector<MeshInstance> meshInstances;
    SkyboxInstance skybox;
    FlyCamera camera;
    void compile();

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
