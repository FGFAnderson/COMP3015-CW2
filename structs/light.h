#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct Light {
    glm::vec3 Direction;
    glm::vec3 La;
    glm::vec3 Ld;
};

#endif // LIGHT_H
