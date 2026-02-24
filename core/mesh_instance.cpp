#include "mesh_instance.h"
#include "helper/texture.h"
#include "trianglemesh.h"

MeshInstance::MeshInstance(const char* obj_filename, const char* diffuse_texture_path, const char* normal_map_path, const std::optional<const char*>& alpha_map_path, const Material& mat, const glm::mat4& transform, bool center)
    : mesh(ObjMesh::load(obj_filename, center, true)),
      diffuseTexture(Texture::loadTexture(diffuse_texture_path)),
      normalMap(Texture::loadTexture(normal_map_path)),
      transform(transform),
      material(mat)
{
    if(alpha_map_path) {
        alphaMap = Texture::loadTexture(*alpha_map_path);
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

    // Load normal map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    prog.setUniform("normalMap", 1);

    // Load alpha map if exists
    prog.setUniform("hasAlphaMap", alphaMap.has_value());
    if(alphaMap.has_value()) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, alphaMap.value());
        prog.setUniform("alphaTex", 2);
    }

    mesh->render();
}
