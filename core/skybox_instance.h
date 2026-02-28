#ifndef SKYBOX_INSTANCE_H
#define SKYBOX_INSTANCE_H

#include <memory>
#include <glm/glm.hpp>
#include "helper/skybox.h"
#include "helper/glslprogram.h"

class SkyboxInstance {
private:
    std::unique_ptr<SkyBox> skybox;
    GLuint cubemapTexture;

public:
    SkyboxInstance();

    void render(GLSLProgram& prog, const glm::mat4& viewMatrix, const glm::mat4& projection);
};

#endif // SKYBOX_INSTANCE_H
