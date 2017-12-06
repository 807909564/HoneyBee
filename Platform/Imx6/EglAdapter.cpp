#include <sys/time.h>
#include "EglAdapter.hpp"

HONEYBEE_BEGIN_NAMESPACE

void signalHandlerWrapper(int signal) {
    UNUSED(signal);
    exit(0);
}

EglAdapter::EglAdapter(HBContext *context) : mContext(context) {
    setenv("FB_MULTIBUFFER", "2", 0);
    signal(SIGINT, &signalHandlerWrapper);
    signal(SIGTERM, &signalHandlerWrapper);
}

EGLBoolean EglAdapter::createWindow(const char *title) {
    UNUSED(title);
    mContext->eglNativeDisplay = (EGLNativeDisplayType)fbGetDisplayByIndex(0);
    mContext->eglNativeWindow = (EGLNativeWindowType)fbCreateWindow(mContext->eglNativeDisplay, 0, 0, 0, 0);
    return GL_TRUE;
}

void EglAdapter::windowLoop() {
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;

    gettimeofday(&t1, &tz);

    while (true) {
        gettimeofday(&t2, &tz);
        deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
        t1 = t2;

        if (mContext->updateFunc != nullptr)
            mContext->updateFunc(mContext, deltatime);
        if (mContext->drawFunc != nullptr)
            mContext->drawFunc(mContext);

        eglSwapBuffers(mContext->eglDisplay, mContext->eglSurface);
    }
}

HONEYBEE_END_NAMESPACE
