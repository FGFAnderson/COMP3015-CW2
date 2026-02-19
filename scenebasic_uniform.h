#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/objmesh.h"
#include "helper/plane.h"
#include <glad/glad.h>
#include <memory>
#include "helper/glslprogram.h"

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    glm::vec3 lightDirection;
    std::unique_ptr<ObjMesh> mesh;
    Plane plane;

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
