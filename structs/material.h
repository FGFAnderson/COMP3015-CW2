#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct Material {
    glm::vec3 Ka;
    glm::vec3 Ks;
    float Shininess;
};

#endif // MATERIAL_H
