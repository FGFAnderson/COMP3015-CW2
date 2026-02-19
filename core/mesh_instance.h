#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "helper/objmesh.h"
#include "helper/glslprogram.h"
#include "structs/material.h"

class MeshInstance {
private:
    std::unique_ptr<ObjMesh> mesh;
    glm::mat4 transform;
    Material material;

public:
    MeshInstance(const char* filename, const Material& mat, const glm::mat4& transform = glm::mat4(1.0f), bool center = true);

    void setTransform(const glm::mat4& t) { transform = t; }
    glm::mat4 getTransform() const { return transform; }

    void setMaterial(const Material& m) { material = m; }
    Material getMaterial() const { return material; }

    void render(GLSLProgram& prog, const glm::mat4& view,
                const glm::mat4& projection);
};

#endif // MESH_INSTANCE_H
