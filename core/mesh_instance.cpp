#include "mesh_instance.h"
#include "helper/texture.h"
#include "trianglemesh.h"

MeshInstance::MeshInstance(const char* objPath, const char* diffuseTexturePath, const std::optional<const char*>& normalMapPath, const std::optional<const char*>& alphaMapPath, const Material& mat, const glm::mat4& transform, bool center)
    : mesh(ObjMesh::load(objPath, center, true)),
      diffuseTexture(Texture::loadTexture(diffuseTexturePath)),
      transform(transform),
      material(mat)
{
    if(normalMapPath) {
        normalMap = Texture::loadTexture(*normalMapPath);
    }
    if(alphaMapPath) {
        alphaMap = Texture::loadTexture(*alphaMapPath);
    }
}

void MeshInstance::render(GLSLProgram& prog, const glm::mat4& view,
                          const glm::mat4& projection) {
    glm::mat4 mv = view * transform;

    prog.setUniform("modelViewMatrix", mv);
    prog.setUniform("normalMatrix",
                    glm::mat3(glm::transpose(glm::inverse(mv))));
    prog.setUniform("mvp", projection * mv);

    // Set material properties
    prog.setUniform("material.ka", material.ka);
    prog.setUniform("material.ks", material.ks);
    prog.setUniform("material.shininess", material.shininess);

    // Load color texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    prog.setUniform("tex", 0);

    prog.setUniform("alphaTest", true);

    // Load normal map if present
    prog.setUniform("hasNormalMap", normalMap.has_value());
    if(normalMap.has_value()) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap.value());
        prog.setUniform("normalMap", 1);
    }

    // Load alpha map if exists
    prog.setUniform("hasAlphaMap", alphaMap.has_value());
    if(alphaMap.has_value()) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, alphaMap.value());
        prog.setUniform("alphaTex", 2);
    }

    mesh->render();
}
