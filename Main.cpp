#include <iostream>
#include "HBEgl.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "HBHelper.hpp"
#include "HBModel.hpp"

typedef struct {
    // Handle to a program object
    GLuint programObject;
    honeybee::HBModel *model = nullptr;
} UserData;

int main() {
    honeybee::HBEgl egl;
    egl.glContext()->userData = malloc(sizeof(UserData));
    egl.createWindow("HoneyBee", 800, 700, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    egl.registerInitFunc([](honeybee::HBContext *context) {
        // Load the vertex/fragment shaders
        auto programObject = honeybee::HBHelper::loadProgramByPath("vertexShader.vs", "fragShader.fs");
        if (programObject == 0) return false;

        // Store the program object
        UserData *userData = (UserData *)(context->userData);
        userData->programObject = programObject;

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(60.0f), (float)context->width / context->height, 0.1f, 800.0f);

        glUseProgram(userData->programObject);
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
        glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

       // auto ourModel = new honeybee::HBModel("../assimp/test/models/OBJ/spider.obj");
        auto ourModel = new honeybee::HBModel("x5/x5.obj");
        userData->model = ourModel;
        glViewport (0, 0, context->width, context->height);

        return true;
    });
    egl.registerDrawFunc([](honeybee::HBContext *context) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        UserData *userData = (UserData *)(context->userData);
        auto model = userData->model;
        if (model) model->draw(userData->programObject);
    });
    egl.registerUpdateFunc([] (honeybee::HBContext *context, float deltaTime) {
        UserData *userData = (UserData *)(context->userData);
        static float time = 0;
        if (time > 360) {
            time = 0;
        } else {
            time += deltaTime * 2;
        }
        glm::mat4 view;
        glm::vec3 viewPos(-200.0f, 200.0f, 200.0f);
        view = glm::lookAt(viewPos,
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(userData->programObject, "viewPos"), 1, glm::value_ptr(viewPos));
    });
    egl.registerShutdownFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        glDeleteProgram(userData->programObject);
        if (userData->model) {
            delete userData->model;
            userData = nullptr;
        }
    });
    return egl.exec();
}
