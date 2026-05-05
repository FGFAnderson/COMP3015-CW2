#include "mesh_instance.h"

MeshInstance::MeshInstance(const char* meshPath, const Material& mat,
                           const glm::mat4& t, bool center)
    : mesh(AssimpMesh::load(meshPath, center)),
    transform(t),
    material(mat)
{}

void MeshInstance::render(GLSLProgram& prog, const glm::mat4& view,
                          const glm::mat4& projection) {
    glm::mat4 mv = view * transform;

    prog.setUniform("modelViewMatrix", mv);
    prog.setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(mv))));
    prog.setUniform("mvp", projection * mv);

    prog.setUniform("material.ka", material.ka);
    prog.setUniform("material.ks", material.ks);
    prog.setUniform("material.shininess", material.shininess);

    prog.setUniform("tex", 0);

    // AssimpMesh::render() binds the correct texture per material group
    mesh->render();
}
