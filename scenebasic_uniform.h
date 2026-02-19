#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <glad/glad.h>
#include <vector>
#include "helper/glslprogram.h"
#include "core/mesh_instance.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    glm::vec3 lightDirection;
    std::vector<MeshInstance> meshInstances;

    void setMatrices();
    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
