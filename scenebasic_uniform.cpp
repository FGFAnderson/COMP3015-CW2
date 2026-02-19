#include "scenebasic_uniform.h"
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

SceneBasic_Uniform::SceneBasic_Uniform() {
    Material rockMaterial;
    rockMaterial.Ka = vec3(0.15f, 0.5f, 0.5f);
    rockMaterial.Ks = vec3(0.1f, 0.1f, 0.1f);
    rockMaterial.Shininess = 2.0f;
    glm::mat4 rockTransform = glm::scale(glm::mat4(1.0f), vec3(0.05f));
    meshInstances.push_back(MeshInstance("./media/models/free_pack_rocks_stylized.obj", "./media/textures/Rocks_Stylized_Color.png", rockMaterial, rockTransform, true));
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    view = glm::lookAt(vec3(0.0f, 10.0f, 20.0f), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
    projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);

    glm::vec4 lightDir = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    lightDirection = glm::normalize(glm::vec3(view * lightDir));
    prog.setUniform("light.Direction", lightDirection);
    prog.setUniform("light.La", 0.2f, 0.2f, 0.2f); // Ambient
    prog.setUniform("light.Ld", 1.0f, 1.0f, 1.0f); // Diffuse
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

    // AI USAGE: refer to ./ai_transcript/vector_looping
    for (auto& meshInstance : meshInstances) {
        meshInstance.render(prog, view, projection);
    }
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(90.0f), (float)w/h, 0.1f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
}
