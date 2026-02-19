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

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    glm::mat3(glm::transpose(glm::inverse(mv))));
    prog.setUniform("MVP", projection * mv);

    prog.setUniform("material.Ka", material.Ka);
    prog.setUniform("material.Ks", material.Ks);
    prog.setUniform("material.Shininess", material.Shininess);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    prog.setUniform("tex", 0);

    mesh->render();
}
