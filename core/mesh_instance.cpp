#include "mesh_instance.h"

MeshInstance::MeshInstance(const char* filename, const Material& mat, const glm::mat4& t, bool center)
    : mesh(ObjMesh::load(filename, center)),
      transform(t),
      material(mat)
{}

void MeshInstance::render(GLSLProgram& prog, const glm::mat4& view,
                          const glm::mat4& projection) {
    glm::mat4 mv = view * transform;

    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    glm::mat3(glm::transpose(glm::inverse(mv))));
    prog.setUniform("MVP", projection * mv);

    prog.setUniform("material.Ka", material.Ka);
    prog.setUniform("material.Kd", material.Kd);
    prog.setUniform("material.Ks", material.Ks);
    prog.setUniform("material.Shininess", material.Shininess);

    mesh->render();
}
