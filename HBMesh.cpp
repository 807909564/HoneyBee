#include <cstddef>

#include "HBMesh.hpp"

HONEYBEE_BEGIN_NAMESPACE

HBMesh::HBMesh(std::vector<HBVertex> &vertices,
               std::vector<unsigned int> &indices,
               std::vector<HBTexture> &textures,
               HBMeshColor &color,
               const std::string &name) {
    mVertices = std::move(vertices);
    mIndices = std::move(indices);
    mTextures = std::move(textures);
    mColor = std::move(color);
    mName = std::move(name);
    setup();
}

void HBMesh::draw(GLuint programObject) {
    glUseProgram(programObject);
    unsigned int diffuseNr{1};
    unsigned int specularNr{1};
    for (auto i = 0; i < mTextures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = mTextures[i].sType;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if(name == "texture_specular") {
            number = std::to_string(specularNr++);
        }
        glUniform1i(glGetUniformLocation(programObject, ("material." + name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, mTextures[i].sId);
    }
    glUniform4fv(glGetUniformLocation(programObject, "material.ambient"), 1, glm::value_ptr(mColor.sAmbientColor));
    glUniform4fv(glGetUniformLocation(programObject, "material.diffuse"), 1, glm::value_ptr(mColor.sDiffuseColor));
    glUniform4fv(glGetUniformLocation(programObject, "material.specular"), 1, glm::value_ptr(mColor.sSpecularColor));

    glUniform3fv(glGetUniformLocation(programObject, "light.pos"), 1, glm::value_ptr(glm::vec3(300.0f, 300.0f, 300.0f)));
    glUniform4fv(glGetUniformLocation(programObject, "light.color"), 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));

    glBindVertexArray(mVaoId);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HBMesh::setup() {
    glGenVertexArrays(1, &mVaoId);
    glGenBuffers(1, &mVboId);
    glGenBuffers(1, &mEboId);

    glBindVertexArray(mVaoId);

    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(HBVertex), &mVertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(HBVertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(HBVertex), (void *)offsetof(HBVertex, sNormal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(HBVertex), (void *)offsetof(HBVertex, sTexCoords));

    glBindVertexArray(0);
}

HONEYBEE_END_NAMESPACE
