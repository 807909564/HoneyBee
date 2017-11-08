#include <iostream>

#include "HBModel.hpp"
#include "HBHelper.hpp"

HONEYBEE_BEGIN_NAMESPACE

HBModel::HBModel(const std::string &path) {
    loadModel(path);
}

void HBModel::draw(GLuint programObject) {
    for (auto &mesh : mMeshes) mesh.draw(programObject);
}

void HBModel::loadModel(const std::string &path) {
    Assimp::Importer importer;
    auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    mModelDirectory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void HBModel::processNode(aiNode *node, const aiScene *scene) {
    for (auto i = 0; i < node->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        mMeshes.push_back(std::move(processMesh(mesh, scene)));
    }

    for (auto i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

HBMesh HBModel::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<HBVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<HBTexture> textures;

    for (auto i = 0; i < mesh->mNumVertices && mesh->mNormals; ++i) {
        HBVertex vertex;
        glm::vec3 v3;
        v3.x = mesh->mVertices[i].x;
        v3.y = mesh->mVertices[i].y;
        v3.z = mesh->mVertices[i].z;
        vertex.sPosition = v3;

        v3.x = mesh->mNormals[i].x;
        v3.y = mesh->mNormals[i].y;
        v3.z = mesh->mNormals[i].z;
        vertex.sNormal = v3;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 v2;
            v2.x = mesh->mTextureCoords[0][i].x;
            v2.y = mesh->mTextureCoords[0][i].y;
            vertex.sTexCoords = v2;
        } else {
            vertex.sTexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(std::move(vertex));
    }

    for (auto i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return std::move(HBMesh(vertices, indices, textures));
}

std::vector<HBTexture> HBModel::loadMaterialTextures(aiMaterial *mat,
                                                     aiTextureType type,
                                                     const std::string &typeName) {
    std::vector<HBTexture> textures;
    for (auto i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);

        //std::string aStr(str.C_Str());
        auto fileName = std::string(str.C_Str()); //aStr.substr(aStr.find_last_of('\\') + 1, aStr.size() - 1);
        if (fileName.substr(0, 2) == "./" || fileName.substr(0, 2) == ".\\") {
            fileName = fileName.substr(2, fileName.size() - 2);
        }

        auto result = std::find_if(mLoadedTextures.begin(),
                                   mLoadedTextures.end(),
                                   [str](const HBTexture &texture) -> bool {
            return std::string(texture.sPath.C_Str()) == str.C_Str();
        });
        if (result != mLoadedTextures.end()) {
            textures.push_back(*result);
        } else {
            HBTexture texture;
            texture.sId = HBHelper::loadTextureFromFile(mModelDirectory + '/' + fileName);
            texture.sType = typeName;
            texture.sPath = str;
            textures.push_back(texture);
            mLoadedTextures.push_back(texture);
        }
    }

    return textures;
}

HONEYBEE_END_NAMESPACE
