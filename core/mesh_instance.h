#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "helper/assimpmesh.h"
#include "helper/glslprogram.h"
#include "structs/material.h"

class MeshInstance {
private:
    std::unique_ptr<AssimpMesh> mesh;
    glm::mat4 transform;
    Material material;

public:
    // Textures are loaded automatically from the model file's material data.
    MeshInstance(const char* meshPath,
                 const Material& mat,
                 const glm::mat4& transform,
                 bool center = false);

    void setTransform(const glm::mat4& t) { transform = t; }
    glm::mat4 getTransform() const { return transform; }

    void setMaterial(const Material& m) { material = m; }
    Material getMaterial() const { return material; }

    void render(GLSLProgram& prog, const glm::mat4& view,
                const glm::mat4& projection);
};

#endif // MESH_INSTANCE_H
