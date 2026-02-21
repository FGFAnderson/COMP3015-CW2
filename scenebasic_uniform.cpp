#include "scenebasic_uniform.h"
#include <cstdlib>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::cerr;
using std::endl;
using glm::mat4;
using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() {
    Material rockMaterial;
    rockMaterial.ka = vec3(0.15f, 0.5f, 0.5f);
    rockMaterial.ks = vec3(0.1f, 0.1f, 0.1f);
    rockMaterial.shininess = 2.0f;
    glm::mat4 rockTransform = glm::scale(glm::mat4(1.0f), vec3(0.05f));
    meshInstances.push_back(MeshInstance("./media/models/free_pack_rocks_stylized.obj", "./media/textures/Rocks_Stylized_Color.png", rockMaterial, rockTransform, true));
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    view = camera.getViewMatrix();
    projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);

    prog.use();
    glm::vec4 lightDir = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    lightDirection = glm::normalize(glm::vec3(view * lightDir));
    prog.setUniform("light.direction", lightDirection);
    prog.setUniform("light.la", vec3(0.2f)); // Ambient
    prog.setUniform("light.ld", vec3(0.5f)); // Diffuse

    prog.setUniform("spotlight.l", vec3(0.9f));
    prog.setUniform("spotlight.exponent", 20.0f);
    prog.setUniform("spotlight.cutoff", glm::radians(50.0f));

    prog.setUniform("fog.maxDist", 50.0f);
    prog.setUniform("fog.minDist", 5.0f);
    prog.setUniform("fog.color", vec3(0.5f));
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

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.use();
    // AI USAGE: refer to ./ai_transcript/vector_looping
    for (auto& meshInstance : meshInstances) {
        meshInstance.render(prog, view, projection);
    }

    skyboxProg.use();
    skybox.render(skyboxProg, view, projection);
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
