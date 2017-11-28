#include <iostream>
#include "HBEgl.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "HBHelper.hpp"
#include "HBModel.hpp"
#include "HBText.hpp"

typedef struct {
    honeybee::HBModel *model = nullptr;
    honeybee::HBText *text = nullptr;
} UserData;

int main() {
    honeybee::HBEgl egl;
    egl.glContext()->userData = malloc(sizeof(UserData));
    egl.createWindow("HoneyBee", 800, 700, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    egl.registerInitFunc([](honeybee::HBContext *context) {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glViewport (0, 0, context->width, context->height);

        UserData *userData = (UserData *)(context->userData);
        userData->model = new honeybee::HBModel("x5/x5.obj", context);
        userData->text = new honeybee::HBText(24, context);
        userData->text->setPosition(10, 670);
        userData->text->setColor(glm::vec4(1.0, 0.0, 0.0, 0.8));

        return true;
    });
    egl.registerDrawFunc([](honeybee::HBContext *context) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        UserData *userData = (UserData *)(context->userData);
        auto model = userData->model;
        if (model) model->draw();
        auto text = userData->text;
        if (text) text->draw();
    });
    egl.registerUpdateFunc([] (honeybee::HBContext *context, float deltaTime) {
        UserData *userData = (UserData *)(context->userData);
        if (userData->model) userData->model->update(deltaTime);
        if (userData->text) {
            userData->text->setText("FPS:" + std::to_string(std::lround(1.0 / deltaTime)));
        }
    });
    egl.registerShutdownFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        if (userData->model) {
            delete userData->model;
        }
        if (userData->text) {
            delete userData->text;
        }
    });
    return egl.exec();
}
