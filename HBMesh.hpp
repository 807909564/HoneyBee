#pragma once

#include <vector>
#include <GLES3/gl3.h>

#include "HBGlobal.hpp"
#include "HBMesh.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

HONEYBEE_BEGIN_NAMESPACE

struct HBVertex {
    glm::vec3 sPosition;
    glm::vec3 sNormal;
    glm::vec2 sTexCoords;
};

struct HBTexture {
    unsigned int sId;
    std::string sType;
    aiString sPath;
};

struct HBMeshColor {
    glm::vec4 sAmbientColor;
    glm::vec4 sDiffuseColor;
    glm::vec4 sSpecularColor;
};

class HBMesh final {
public:
    explicit HBMesh(std::vector<HBVertex> &vertices,
                    std::vector<unsigned int> &indices,
                    std::vector<HBTexture> &textures,
                    HBMeshColor &color,
                    const std::string &name);

    void draw(GLuint programObject);

private:
    void setup();

private:
    std::vector<HBVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<HBTexture> mTextures;
    HBMeshColor mColor;
    std::string mName;
    unsigned int mVaoId{0};
    unsigned int mVboId{0};
    unsigned int mEboId{0};
};

HONEYBEE_END_NAMESPACE
