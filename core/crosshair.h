#pragma once
#include <glad/glad.h>
#include <string>
#include "glslprogram.h"

class Crosshair {
public:
    void init(int width, int height, const std::string& texturePath);
    void render();
private:
    GLSLProgram prog;
    GLuint vao = 0, vbo = 0, ebo = 0, tex = 0;
};
