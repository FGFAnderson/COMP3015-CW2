#include "scenebasic_uniform.h"
#include "objmesh.h"
#include "trianglemesh.h"
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


using glm::mat4;
using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100) {
    mesh=ObjMesh::load("./pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 2.0f, 2.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    // Set directional light (direction in eye space)
    glm::vec4 lightDir = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    lightDirection = glm::normalize(glm::vec3(view * lightDir));
    prog.setUniform("light.Direction", lightDirection);
    prog.setUniform("light.La", 0.2f, 0.2f, 0.2f);  // Ambient
    prog.setUniform("light.Ld", 1.0f, 1.0f, 1.0f);  // Diffuse
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
	float angle = t * 0.5f;
	float x = 3.0f * cos(angle);
	float z = 3.0f * sin(angle);

	glm::vec4 lightPos = glm::vec4(x, 2.0f, z, 0.0f);
	lightDirection = glm::normalize(glm::vec3(view * lightPos));
	prog.setUniform("light.Direction", lightDirection);
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setMatrices();

    mesh->render();

    plane.render();
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

    prog.setUniform("material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("material.Kd", 1.0f, 0.5f, 0.5f);
    prog.setUniform("material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("material.Shininess", 100.0f);
}
