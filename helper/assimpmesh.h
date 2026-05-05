#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// Loads any Assimp-supported format (OBJ, FBX, GLTF, ...).
class AssimpMesh {
public:
    static std::unique_ptr<AssimpMesh> load(const char* path, bool center = false);
    ~AssimpMesh();

    AssimpMesh(const AssimpMesh&) = delete;
    AssimpMesh& operator=(const AssimpMesh&) = delete;
    AssimpMesh(AssimpMesh&&) noexcept;

    void render() const;

    GLuint getVao() const { return vao; }
    int getNumIndices() const { return totalIndexCount; }

private:
    AssimpMesh() = default;

    struct MaterialGroup {
        GLuint diffuse;
        int indexOffset;
        int indexCount;
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    int totalIndexCount = 0;

    std::vector<MaterialGroup> groups;
    std::vector<GLuint> ownedTextures;
};
