#include <iostream>

#include "HBModel.hpp"
#include "HBHelper.hpp"

HONEYBEE_BEGIN_NAMESPACE

HBModel::HBModel(const std::string &path, const HBContext *context) {
    mProgramObject = honeybee::HBHelper::loadProgramByPath("Shader/vertexShader.vs", "Shader/fragShader.fs");
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), (float)context->width / context->height, 0.1f, 800.0f);

    glUseProgram(mProgramObject);
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
    glUniformMatrix4fv(glGetUniformLocation(mProgramObject, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(mProgramObject, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    loadModel(path);
}

void HBModel::draw() {
    for (auto &mesh : mMeshes) mesh.draw(mProgramObject);
}

void HBModel::update(float deltaTime) {
    static float time = 0.0;
    if (time > 360) {
        time = 0;
    } else {
        time += deltaTime * 2;
    }
    glm::mat4 view;
    glm::vec3 viewPos(-200.0f * sin(time), 200.0f, 200.0f * cos(time));
    view = glm::lookAt(viewPos,
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));

    glUseProgram(mProgramObject);
    glUniformMatrix4fv(glGetUniformLocation(mProgramObject, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(glGetUniformLocation(mProgramObject, "viewPos"), 1, glm::value_ptr(viewPos));
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
    for (auto i = 0u; i < node->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        mMeshes.push_back(std::move(processMesh(mesh, scene)));
    }

    for (auto i = 0u; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

HBMesh HBModel::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<HBVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<HBTexture> textures;

    for (auto i = 0u; i < mesh->mNumVertices && mesh->mNormals; ++i) {
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

    for (auto i = 0u; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (auto j = 0u; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    HBMeshColor meshColor;
    auto material = scene->mMaterials[mesh->mMaterialIndex];
    auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    meshColor.sAmbientColor = loadMaterialColor(material, AI_MATKEY_COLOR_AMBIENT);
    meshColor.sDiffuseColor = loadMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE);
    meshColor.sSpecularColor = loadMaterialColor(material, AI_MATKEY_COLOR_SPECULAR);

    return HBMesh(vertices, indices, textures, meshColor, mesh->mName.C_Str());
}

std::vector<HBTexture> HBModel::loadMaterialTextures(aiMaterial *mat,
                                                     aiTextureType type,
                                                     const std::string &typeName) {
    std::vector<HBTexture> textures;
    for (auto i = 0u; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::string aStr(str.C_Str());
        auto fileName = aStr.substr(aStr.find_last_of('/') + 1, aStr.size());
        auto result = std::find_if(mLoadedTextures.begin(),
                                   mLoadedTextures.end(),
                                   [str](const HBTexture &texture) -> bool {
            return std::string(texture.sPath.C_Str()) == str.C_Str();
        });
        if (result != mLoadedTextures.end()) {
            textures.push_back(*result);
        } else {
            HBTexture texture;
            std::cout << mModelDirectory + '/' + fileName << std::endl;
            texture.sId = HBHelper::loadTextureFromFile(mModelDirectory + '/' + fileName);
            texture.sType = typeName;
            texture.sPath = str;
            textures.push_back(texture);
            mLoadedTextures.push_back(texture);
        }
    }

    return textures;
}

glm::vec4 HBModel::loadMaterialColor(aiMaterial *mat,
                                     const char* pKey,
                                     unsigned int type,
                                     unsigned int index) {
    aiColor4D color;
    if (AI_SUCCESS == aiGetMaterialColor(mat, pKey, type, index, &color)) {
        return glm::vec4(color.r, color.g, color.b, color.a);
    }

    return glm::vec4(0.0, 0.0, 0.0, 1.0);
}

HONEYBEE_END_NAMESPACE
