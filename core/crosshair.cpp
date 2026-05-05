#include "crosshair.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <cstddef>

void Crosshair::init(int width, int height, const std::string& texturePath) {
    prog.compileShader("shader/crosshair.vert");
    prog.compileShader("shader/crosshair.frag");
    prog.link();
    prog.use();
    prog.setUniform("screenTexture", 0);

    tex = Texture::loadTexture(texturePath);

    float sizeY = 0.05f;
    float sizeX = sizeY / (float(width) / float(height));

    struct Vertex { glm::vec2 pos; glm::vec2 texCoord; };
    Vertex verts[] = {
        { glm::vec2(-sizeX, -sizeY), glm::vec2(0.0f, 0.0f) },
        { glm::vec2(sizeX, -sizeY), glm::vec2(1.0f, 0.0f) },
        { glm::vec2(sizeX, sizeY), glm::vec2(1.0f, 1.0f) },
        { glm::vec2(-sizeX, sizeY), glm::vec2(0.0f, 1.0f) },
    };
    GLuint indices[] = { 0, 1, 2, 0, 2, 3 };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Crosshair::render() {
    glDisable(GL_DEPTH_TEST);
    prog.use();
    glBindTextureUnit(0, tex);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}
