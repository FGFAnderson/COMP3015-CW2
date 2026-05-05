#pragma once
#include <glad/glad.h>
#include "glslprogram.h"

class PostProcessPipeline {
public:
    void init(int width, int height);
    void beginScene();
    void process(bool applyBlur);
private:
    GLSLProgram blurHProg, blurVProg, passthroughProg;
    GLuint sceneFBO = 0, sceneTex = 0, sceneDepth = 0;
    GLuint intermediateFBO = 0, intermediateTex = 0;
    GLuint screenVao = 0;
    int width, height;

    void setupFBO();
    void pass1_renderScene();
    void pass2_verticalBlur();
    void pass3_horizontalBlur();
};
