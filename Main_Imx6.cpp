#include <iostream>
#include "HBEgl.hpp"
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"
#include "HBHelper.hpp"
#include "HBModel.hpp"

typedef struct {
    honeybee::HBModel *model = nullptr;

} UserData;

int main() {
    honeybee::HBEgl egl;
    egl.glContext()->userData = new UserData;
    egl.createWindow("HoneyBee", 1440, 540, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    egl.registerInitFunc([](honeybee::HBContext *context) {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glViewport (0, 0, context->width, context->height);
        auto userData = static_cast<UserData *>(context->userData);
        userData->model = new honeybee::HBModel("/tmp/Model/x5/x5.obj", context);
        return true;
    });
    egl.registerDrawFunc([](honeybee::HBContext *context) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto userData = static_cast<UserData *>(context->userData);
        auto model = userData->model;
        if (model) model->draw();
    });
    egl.registerUpdateFunc([] (honeybee::HBContext *context, float deltaTime) {
        auto userData = (UserData *)(context->userData);
        if (userData->model) userData->model->update(deltaTime);
    });
    egl.registerShutdownFunc([](honeybee::HBContext *context) {
        auto userData = static_cast<UserData *>(context->userData);
        honeybee::destroy(userData->model);
        honeybee::destroy(userData);
    });
    return egl.exec();
}
