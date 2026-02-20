#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

struct Light {
    glm::vec3 direction;
    glm::vec3 la;
    glm::vec3 ld;
};

#endif // LIGHT_H
