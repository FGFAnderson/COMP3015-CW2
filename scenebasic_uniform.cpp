#include "scenebasic_uniform.h"
#include "trianglemesh.h"

#include <cstdio>
#include <cstdlib>

#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "helper/glutils.h"

using glm::mat4;
using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 200, 200) {}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
	glm::vec4 lightLocation = glm::vec4(5.0f * cos(t), 2.0f, 3.0f * sin(t), 1.0f);
	lightPosition = view * lightLocation;
	prog.setUniform("LightPosition", lightPosition);
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setMatrices();
    torus.render();

    glBindVertexArray(0);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(70.0f), (float)w/h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    glm::mat4 mv = view * model;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(glm::transpose(glm::inverse(mv))));
    prog.setUniform("MVP", projection * mv);

    prog.setUniform("Kd", 0.2f, 0.5f, 0.3f);
    prog.setUniform("Ks", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Ld", 1.0f, 1.0f, 1.0f);
    prog.setUniform("Shininess", 70.0f);
    prog.setUniform("LightPosition", lightPosition);
}
