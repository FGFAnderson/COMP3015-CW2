## Texture Loading:

how to load textures from FBX files in Assimp?
7:25
Claude responded: I'll walk you through loading textures from FBX files using Assimp.

I'll walk you through loading textures from FBX files using Assimp. Here's a comprehensive guide:
Basic Setup
cpp

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

Loading the Scene
cpp

Assimp::Importer importer;
const aiScene* scene = importer.ReadFile("model.fbx",
    aiProcess_Triangulate |
    aiProcess_FlipUVs |
    aiProcess_CalcTangentSpace);

if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
    return;
}

Accessing Textures

FBX files store texture information in materials. Here's how to extract them:
cpp

void LoadTextures(const aiScene* scene, const std::string& modelDir) {
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* material = scene->mMaterials[i];
        
        // Get number of textures for each type
        unsigned int diffuseCount = material->GetTextureCount(aiTextureType_DIFFUSE);
        unsigned int normalCount = material->GetTextureCount(aiTextureType_NORMAL);
        unsigned int specularCount = material->GetTextureCount(aiTextureType_SPECULAR);
        
        std::cout << "Material " << i << ":\n";
        std::cout << "  Diffuse textures: " << diffuseCount << "\n";
        std::cout << "  Normal textures: " << normalCount << "\n";
        std::cout << "  Specular textures: " << specularCount << "\n";
        
        // Load diffuse textures
        for (unsigned int j = 0; j < diffuseCount; ++j) {
            aiString texturePath;
            material->GetTexture(aiTextureType_DIFFUSE, j, &texturePath);
            std::string fullPath = modelDir + "/" + texturePath.C_Str();
            std::cout << "  Diffuse texture: " << fullPath << "\n";
            // Load texture from disk here
        }
    }
}

Common Texture Types to Check
cpp

enum TextureTypeMap {
    aiTextureType_DIFFUSE,      // Base color
    aiTextureType_SPECULAR,     // Specular highlights
    aiTextureType_AMBIENT,      // Ambient occlusion
    aiTextureType_EMISSIVE,     // Self-illumination
    aiTextureType_HEIGHT,       // Height map
    aiTextureType_NORMALS,      // Normal map
    aiTextureType_SHININESS,    // Shininess/roughness
    aiTextureType_OPACITY,      // Alpha/transparency
    aiTextureType_DISPLACEMENT, // Displacement
    aiTextureType_LIGHTMAP,     // Baked lighting
    aiTextureType_REFLECTION    // Environment map
};

Important Considerations for FBX

⚠️ Embedded vs. External Textures:

    External: FBX files reference textures by relative/absolute path
        Assimp returns the path string
        You must load the texture file from disk yourself
    Embedded: Some FBX files have textures baked in
        Accessible via scene->mTextures array
        Each aiTexture* is a raw image buffer

cpp

// Check for embedded textures
if (scene->mNumTextures > 0) {
    std::cout << "Found " << scene->mNumTextures << " embedded textures\n";
    
    for (unsigned int i = 0; i < scene->mNumTextures; ++i) {
        aiTexture* texture = scene->mTextures[i];
        std::cout << "Texture " << i << ": " << texture->mFilename.C_Str() 
                  << " (" << texture->mWidth << "x" << texture->mHeight << ")\n";
        
        // texture->pcData points to raw pixel data
        // texture->mFilename contains the original filename
    }
}

Full Example: Extract All Textures
cpp

void ExtractAllTextures(const aiScene* scene, const std::string& modelDir) {
    std::vector<std::string> texturePaths;
    
    for (unsigned int m = 0; m < scene->mNumMaterials; ++m) {
        aiMaterial* material = scene->mMaterials[m];
        
        // Check all texture types
        for (unsigned int t = 0; t < aiTextureType_UNKNOWN; ++t) {
            unsigned int count = material->GetTextureCount((aiTextureType)t);
            
            for (unsigned int i = 0; i < count; ++i) {
                aiString path;
                material->GetTexture((aiTextureType)t, i, &path);
                
                std::string fullPath = modelDir + "/" + path.C_Str();
                texturePaths.push_back(fullPath);
            }
        }
    }
    
    // Remove duplicates
    std::sort(texturePaths.begin(), texturePaths.end());
    texturePaths.erase(std::unique(texturePaths.begin(), texturePaths.end()), 
                       texturePaths.end());
    
    std::cout << "Total unique textures: " << texturePaths.size() << "\n";
    for (const auto& p : texturePaths) {
        std::cout << "  " << p << "\n";
    }
}

With Image Loading (e.g., stb_image)
cpp

#include "stb_image.h"

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, 
                                          std::string typeName, const std::string& modelDir) {
    std::vector<Texture> textures;
    
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        
        std::string path = modelDir + "/" + str.C_Str();
        
        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        
        if (data) {
            // Create GL texture and bind
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            stbi_image_free(data);
            
            textures.push_back({textureID, typeName, path});
        }
    }
    
    return textures;
}

Key Points

    Relative paths — FBX texture paths are often relative; construct full paths using the model directory
    Check both sources — Look for external paths AND check scene->mTextures for embedded data
    Post-processing flags — aiProcess_FlipUVs flips V coordinate (useful for OpenGL)
    Texture types vary — Not all FBX files use the same texture naming conventions
    Error handling — Always check if texture files exist before loading

This should get you up and running with texture extraction from FBX files!
