#include "post_process.h"
#include <random>

void PostProcessPipeline::init(int width, int height) {
    this->width = width;
    this->height = height;

    blurHProg.compileShader("shader/fullscreen.vert");
    blurHProg.compileShader("shader/blur_horizontal.frag");
    blurHProg.link();
    blurVProg.compileShader("shader/fullscreen.vert");
    blurVProg.compileShader("shader/blur_vertical.frag");
    blurVProg.link();
    passthroughProg.compileShader("shader/fullscreen.vert");
    passthroughProg.compileShader("shader/passthrough.frag");
    passthroughProg.link();

    // AI USAGE: refer to ai_transcript/gaussian_function.md
    // Compute Gaussian weights using normal distribution
    float sigma = 1.33f;
    float weights[5];
    float sum = 0.0f;

    std::mt19937 gen(0);
    std::normal_distribution<float> dist(0.0f, sigma);

    weights[0] = dist(gen);
    sum = weights[0];
    for (int i = 1; i < 5; i++) {
        weights[i] = dist(gen);
        sum += 2.0f * weights[i];
    }

    // Normalize weights to sum to 1.0
    for (int i = 0; i < 5; i++) {
        weights[i] /= sum;
    }

    for (GLSLProgram* p : { &blurHProg, &blurVProg, &passthroughProg }) {
        p->use();
        p->setUniform("screenTexture", 0);
        for (int i = 0; i < 5; i++)
            p->setUniform(("weight[" + std::to_string(i) + "]").c_str(), weights[i]);
    }


    setupFBO();
    glGenVertexArrays(1, &screenVao);
}

void PostProcessPipeline::setupFBO() {
    // Render scene to texture
    glGenTextures(1, &sceneTex);
    glBindTexture(GL_TEXTURE_2D, sceneTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &sceneDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sceneDepth);

    // Intermediate texture for blur passes
    glGenTextures(1, &intermediateTex);
    glBindTexture(GL_TEXTURE_2D, intermediateTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTex, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessPipeline::beginScene() {
    // Pass 1: Render the scene to a texture
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessPipeline::process(bool applyBlur) {
    glBindVertexArray(screenVao);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    if (applyBlur) {
        pass2_verticalBlur();
        pass3_horizontalBlur();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        passthroughProg.use();
        glBindTextureUnit(0, sceneTex);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBindVertexArray(0);
}

// Blur code is inspired by Lecture 5 and Lab 5
void PostProcessPipeline::pass2_verticalBlur() {
    // Apply vertical blur
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    blurVProg.use();
    glBindTextureUnit(0, sceneTex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void PostProcessPipeline::pass3_horizontalBlur() {
    // Apply horizontal blur
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    blurHProg.use();
    glBindTextureUnit(0, intermediateTex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
