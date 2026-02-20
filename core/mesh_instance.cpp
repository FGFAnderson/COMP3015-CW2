#include "mesh_instance.h"
#include "helper/texture.h"

MeshInstance::MeshInstance(const char* obj_filename, const char* texture_path, const Material& mat, const glm::mat4& transform, bool center)
    : mesh(ObjMesh::load(obj_filename, center)),
      texture(Texture::loadTexture(texture_path)),
      transform(transform),
      material(mat)
{
}

void MeshInstance::render(GLSLProgram& prog, const glm::mat4& view,
                          const glm::mat4& projection) {
    glm::mat4 mv = view * transform;

    prog.setUniform("modelViewMatrix", mv);
    prog.setUniform("normalMatrix",
                    glm::mat3(glm::transpose(glm::inverse(mv))));
    prog.setUniform("mvp", projection * mv);

    prog.setUniform("material.ka", material.ka);
    prog.setUniform("material.ks", material.ks);
    prog.setUniform("material.shininess", material.shininess);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    prog.setUniform("tex", 0);

    mesh->render();
}
