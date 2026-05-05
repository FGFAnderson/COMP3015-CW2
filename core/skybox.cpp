#include "skybox.h"
#include "texture.h"

void SkyboxRenderer::init(const std::string& cubeMapPath) {
    mesh = std::make_unique<SkyBox>(500.0f);
    tex = Texture::loadCubeMap(cubeMapPath);

    prog.compileShader("shader/skybox.vert");
    prog.compileShader("shader/skybox.frag");
    prog.link();
}

void SkyboxRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    glDepthFunc(GL_LEQUAL);
    prog.use();
    prog.setUniform("view", glm::mat4(glm::mat3(view)));
    prog.setUniform("projection", projection);
    prog.setUniform("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    mesh->render();
    glDepthFunc(GL_LESS);
}
