#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ka;
    glm::vec3 ks;
    float shininess;
};

#endif // MATERIAL_H
