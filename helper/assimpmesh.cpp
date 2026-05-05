#include "assimpmesh.h"
#include "texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <utility>
#include <stdexcept>
#include <string>
#include <cstddef>

// Inspired by LearnOpenGLs Assimp guide https://learnopengl.com/Model-Loading/Assimp
// AI_USAGE for texture loading, Refer to: ai_transcript/texture_loading.md
static std::string dirOf(const std::string& path) {
    auto pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? "./" : path.substr(0, pos + 1);
}

static std::string filenameOf(const std::string& path) {
    auto pos = path.find_last_of("/\\");
    return (pos == std::string::npos) ? path : path.substr(pos + 1);
}


static GLuint loadMatTex(const aiMaterial* mat, aiTextureType type, const std::string& texDir) {
    if (mat->GetTextureCount(type) == 0) return 0;
    aiString aiPath;
    mat->GetTexture(type, 0, &aiPath);
    return Texture::loadTexture((texDir + filenameOf(aiPath.C_Str())).c_str());
}

std::unique_ptr<AssimpMesh> AssimpMesh::load(const char* path, bool center) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes |
        aiProcess_PreTransformVertices);

    if (!scene || !scene->mRootNode || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        throw std::runtime_error(std::string("AssimpMesh: failed to load '") + path + "': " + importer.GetErrorString());

    // look in media/textures/ relative to model dir
    std::string dir = dirOf(path);
    std::string texDir = dir + "../textures/";
    std::vector<GLuint> matTextures(scene->mNumMaterials);
    auto out = std::unique_ptr<AssimpMesh>(new AssimpMesh());

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial* mat = scene->mMaterials[i];
        GLuint diff = loadMatTex(mat, aiTextureType_DIFFUSE, texDir);
        if (!diff) diff = loadMatTex(mat, aiTextureType_BASE_COLOR, texDir);
        matTextures[i] = diff;
        out->ownedTextures.push_back(diff);
    }

    struct Vertex { glm::vec3 pos, norm; glm::vec2 tex; };
    std::vector<Vertex> vertices;
    std::vector<GLuint> allIndices;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        const aiMesh* mesh = scene->mMeshes[m];
        auto baseIndex = static_cast<GLuint>(vertices.size());

        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            vertices.push_back({
                { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z },
                { mesh->mNormals[v].x,  mesh->mNormals[v].y,  mesh->mNormals[v].z  },
                mesh->mTextureCoords[0]
                    ? glm::vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y)
                    : glm::vec2(0.0f)
            });
        }

        MaterialGroup g;
        g.diffuse = matTextures[mesh->mMaterialIndex];
        g.indexOffset = static_cast<int>(allIndices.size());
        g.indexCount = static_cast<int>(mesh->mNumFaces) * 3;
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            for (unsigned int k = 0; k < 3; k++)
                allIndices.push_back(baseIndex + mesh->mFaces[f].mIndices[k]);
        out->groups.push_back(g);
    }

    if (center && !vertices.empty()) {
        glm::vec3 lo = vertices[0].pos, hi = vertices[0].pos;
        for (auto& v : vertices) { lo = glm::min(lo, v.pos); hi = glm::max(hi, v.pos); }
        glm::vec3 mid = (lo + hi) * 0.5f;
        for (auto& v : vertices) v.pos -= mid;
    }
    out->totalIndexCount = static_cast<int>(allIndices.size());

    // Upload to GPU
    glGenVertexArrays(1, &out->vao);
    glBindVertexArray(out->vao);

    glGenBuffers(1, &out->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, out->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));

    glGenBuffers(1, &out->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndices.size() * sizeof(GLuint), allIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    return out;
}

void AssimpMesh::render() const {
    glBindVertexArray(vao);
    for (auto& g : groups) {
        glBindTextureUnit(0, g.diffuse);
        glDrawElements(GL_TRIANGLES, g.indexCount, GL_UNSIGNED_INT,
                       reinterpret_cast<void*>(g.indexOffset * sizeof(GLuint)));
    }
    glBindVertexArray(0);
}

AssimpMesh::AssimpMesh(AssimpMesh&& o) noexcept
    : vao(std::exchange(o.vao, 0))
    , vbo(std::exchange(o.vbo, 0))
    , ebo(std::exchange(o.ebo, 0))
    , totalIndexCount(std::exchange(o.totalIndexCount, 0))
    , groups(std::move(o.groups))
    , ownedTextures(std::move(o.ownedTextures)) {}

AssimpMesh::~AssimpMesh() {
    for (GLuint t : ownedTextures) glDeleteTextures(1, &t);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}
