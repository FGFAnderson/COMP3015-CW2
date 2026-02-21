#include "skybox_instance.h"
#include "helper/texture.h"
#include <glad/glad.h>

// https://learnopengl.com/Advanced-OpenGL/Cubemaps, skybox was very easy to implement but I still consulted learnopengl and found some useful stuff
SkyboxInstance::SkyboxInstance() : skybox(new SkyBox(500.0f)), cubemapTexture(0)
{
    cubemapTexture = Texture::loadCubeMap("./media/textures/skybox/skybox", ".png");
}

void SkyboxInstance::render(GLSLProgram& prog, const glm::mat4& view_in, const glm::mat4& projection)
{
    glDepthFunc(GL_LEQUAL);

    glm::mat4 view = glm::mat4(glm::mat3(view_in));

    prog.setUniform("projection", projection);
    prog.setUniform("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    prog.setUniform("skybox", 0);

    skybox->render();

    glDepthFunc(GL_LESS);
}
