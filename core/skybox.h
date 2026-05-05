#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "glslprogram.h"
#include "helper/skybox.h"

class SkyboxRenderer {
public:
    void init(const std::string& cubeMapPath);
    void render(const glm::mat4& view, const glm::mat4& projection);
private:
    GLSLProgram             prog;
    std::unique_ptr<SkyBox> mesh;
    GLuint                  tex = 0;
};
