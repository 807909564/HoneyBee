#include "EglAdapter.hpp"

HONEYBEE_BEGIN_NAMESPACE

EglAdapter::EglAdapter(HBContext *context) : mContext(context) {
    signal(SIGINT, &signal_handler);
    signal(SIGTERM, &signal_handler);
}

EGLBoolean EglAdapter::createWindow(const char *title) {
    mContext->eglNativeWindow = (EGLNativeWindowType)fbCreateWindow((EGLNativeDisplayType)fbGetDisplayByIndex(0), 0, 0, 0, 0);
}

void EglAdapter::windowLoop() {
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;

    gettimeofday(&t1, &tz);

    while (!mDone) {
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

void EglAdapter::signalHandler(int signal) {
    mDone = 1;
}

HONEYBEE_END_NAMESPACE
