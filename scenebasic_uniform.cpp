#include "scenebasic_uniform.h"
#include <cmath>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using std::cerr;
using std::endl;
using glm::mat4;
using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() {
    Material naturalMaterial;
    naturalMaterial.ka = vec3(0.15f, 0.15f, 0.12f);
    naturalMaterial.ks = vec3(0.05f, 0.05f, 0.05f);
    naturalMaterial.shininess = 8.0f;


    {
        auto t = glm::translate(glm::mat4(1.0f), vec3(-10.0f, 0.0f, -12.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.0f));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_1_Bark_NormalTree.obj",
            "./media/textures/Bark_NormalTree.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_1_Leaves_NormalTree.obj",
            "./media/textures/Leaves_NormalTree_C.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
    }
    {
        auto t = glm::translate(glm::mat4(1.0f), vec3(10.0f, 0.0f, -12.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.0f));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_2_Bark_NormalTree.obj",
            "./media/textures/Bark_NormalTree.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_2_Leaves_NormalTree.obj",
            "./media/textures/Leaves_NormalTree_C.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
    }
    {
        auto t = glm::translate(glm::mat4(1.0f), vec3(-18.0f, 0.0f, -18.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.5f));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_3_Bark_NormalTree.obj",
            "./media/textures/Bark_NormalTree.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_3_Leaves_NormalTree.obj",
            "./media/textures/Leaves_NormalTree_C.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
    }
    {
        auto t = glm::translate(glm::mat4(1.0f), vec3(18.0f, 0.0f, -18.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.5f));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_4_Bark_NormalTree.obj",
            "./media/textures/Bark_NormalTree.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
        meshInstances.push_back(MeshInstance("./media/models/CommonTree_4_Leaves_NormalTree.obj",
            "./media/textures/Leaves_NormalTree_C.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
    }
    {
        auto t = glm::translate(glm::mat4(1.0f), vec3(-5.0f, 0.0f, -24.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.0f));
        meshInstances.push_back(MeshInstance("./media/models/Pine_1_Bark_NormalTree.obj",
            "./media/textures/Bark_NormalTree.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
        meshInstances.push_back(MeshInstance("./media/models/Pine_1_Leaves_Pine.obj",
            "./media/textures/Leaf_Pine_C.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
    }
    {
        auto t = glm::translate(glm::mat4(1.0f), vec3(5.0f, 0.0f, -26.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.0f));
        meshInstances.push_back(MeshInstance("./media/models/Pine_2_Bark_NormalTree.obj",
            "./media/textures/Bark_NormalTree.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
        meshInstances.push_back(MeshInstance("./media/models/Pine_2_Leaves_Pine.obj",
            "./media/textures/Leaf_Pine_C.png", "./media/textures/Bark_NormalTree_Normal.png",
            std::nullopt, naturalMaterial, t, false));
    }

}

void SceneBasic_Uniform::setupFBO() {
    glGenFramebuffers(1, &hdrFBO);

    // Create HDR color texture
    glGenTextures(1, &hdrColorTexture);
    glBindTexture(GL_TEXTURE_2D, hdrColorTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create depth renderbuffer
    glGenRenderbuffers(1, &hdrDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);

    // Attach to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "HDR Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create blur FBO and textures
    glGenFramebuffers(1, &blurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);

    // Create blur texture 1
    glGenTextures(1, &blurTexture1);
    glBindTexture(GL_TEXTURE_2D, blurTexture1);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create blur texture 2
    glGenTextures(1, &blurTexture2);
    glBindTexture(GL_TEXTURE_2D, blurTexture2);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture1, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Blur Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::setupQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // TexCoord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SceneBasic_Uniform::renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    setupFBO();
    setupQuad();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    view = camera.getViewMatrix();
    projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);

    prog.use();
    glm::vec4 lightDir = glm::vec4(0.6f, 1.0f, 0.4f, 0.0f);
    lightDirection = glm::normalize(glm::vec3(view * lightDir));
    prog.setUniform("light.direction", lightDirection);
    prog.setUniform("light.la", vec3(0.2f, 0.18f, 0.15f));
    prog.setUniform("light.ld", vec3(0.85f, 0.75f, 0.6f));

    prog.setUniform("spotlight.l", vec3(2.5f, 2.0f, 1.5f));
    prog.setUniform("spotlight.exponent", 20.0f);
    prog.setUniform("spotlight.cutoff", glm::radians(50.0f));

    prog.setUniform("fog.maxDist", 120.0f);
    prog.setUniform("fog.minDist", 30.0f);
    prog.setUniform("fog.color", vec3(0.6f, 0.75f, 0.8f));

    // Setup Gaussian blur weights
    float weights[5], sum, sigma2 = 8.0f;
    weights[0] = gauss(0, sigma2);
    sum = weights[0];
    for(int i = 1; i < 5; i++) {
        weights[i] = gauss(float(i), sigma2);
        sum += 2 * weights[i];
    }

    // Set uniforms for both blur programs
    blurVerticalProg.use();
    for(int i = 0; i < 5; i++) {
        std::string uniName = "weight[" + std::to_string(i) + "]";
        float val = weights[i] / sum;
        blurVerticalProg.setUniform(uniName.c_str(), val);
    }

    blurHorizontalProg.use();
    for(int i = 0; i < 5; i++) {
        std::string uniName = "weight[" + std::to_string(i) + "]";
        float val = weights[i] / sum;
        blurHorizontalProg.setUniform(uniName.c_str(), val);
    }
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/lighting.vert");
		prog.compileShader("shader/lighting.frag");
		prog.link();

		skyboxProg.compileShader("shader/skybox.vert");
		skyboxProg.compileShader("shader/skybox.frag");
		skyboxProg.link();

		toneMapProg.compileShader("shader/tonemapping.vert");
		toneMapProg.compileShader("shader/tonemapping.frag");
		toneMapProg.link();

		blurVerticalProg.compileShader("shader/blur.vert");
		blurVerticalProg.compileShader("shader/blur_vertical.frag");
		blurVerticalProg.link();

		blurHorizontalProg.compileShader("shader/blur.vert");
		blurHorizontalProg.compileShader("shader/blur_horizontal.frag");
		blurHorizontalProg.link();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
	view = camera.getViewMatrix();

	// Make spotlight follow camera and face direction user is looking
	glm::vec3 camPos = camera.getPosition();
	glm::vec3 spotposition = glm::vec3(view * glm::vec4(camPos, 1.0f));
	glm::vec3 camForward = camera.getForward();
	glm::vec3 spotDirection = glm::normalize(glm::vec3(view * glm::vec4(camForward, 0.0f)));
	prog.use();
	prog.setUniform("spotlight.position", spotposition);
	prog.setUniform("spotlight.direction", spotDirection);
}

void SceneBasic_Uniform::renderScene()
{
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.use();

    // AI USAGE: refer to ./ai_transcript/vector_looping
    for (auto& meshInstance : meshInstances) {
        meshInstance.render(prog, view, projection);
    }

    skyboxProg.use();
    skybox.render(skyboxProg, view, projection);
}

void SceneBasic_Uniform::computeLogAverageLuminance()
{
    int size = width * height;
    glBindTexture(GL_TEXTURE_2D, hdrColorTexture);
    std::vector<GLfloat> texData(size * 3);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());

    float sum = 0.0f;
    for (int i = 0; i < size; i ++) {
        float luminance = glm::dot(
            vec3(texData[i * 3 + 0], texData[i * 3 + 1], texData[i * 3 + 2]),
            vec3(0.2126f, 0.7152f, 0.0722f)
        );

        sum += logf(luminance + 0.0001f);
    }

    logAverageLuminance = exp(sum / (width * height));
}

void SceneBasic_Uniform::blurVertical()
{
    blurVerticalProg.use();
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture1, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorTexture);
    blurVerticalProg.setUniform("texture0", 0);

    renderQuad();
}

void SceneBasic_Uniform::blurHorizontal()
{
    blurHorizontalProg.use();
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture2, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurTexture1);
    blurHorizontalProg.setUniform("texture0", 0);

    renderQuad();
}

void SceneBasic_Uniform::tonemap()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    toneMapProg.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrColorTexture);
    toneMapProg.setUniform("hdrBuffer", 0);
    toneMapProg.setUniform("logAverageLum", logAverageLuminance);

    renderQuad();

    glEnable(GL_DEPTH_TEST);
}

float SceneBasic_Uniform::gauss(float x, float sigma2)
{
    double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
    double expon = -(x * x) / (2.0 * sigma2);
    return (float)(coeff * exp(expon));
}

void SceneBasic_Uniform::render()
{
    renderScene();
    computeLogAverageLuminance();
    //blurVertical();
    //blurHorizontal();
    tonemap();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(90.0f), (float)w/h, 0.1f, 1000.0f);
}

void SceneBasic_Uniform::processInput(GLFWwindow* window, float deltaTime) {
    camera.processInput(window, deltaTime);
}

void SceneBasic_Uniform::processMouseMovement(double xpos, double ypos) {
    camera.processMouseMovement(xpos, ypos);
}
