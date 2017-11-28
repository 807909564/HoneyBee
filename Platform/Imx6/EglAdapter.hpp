#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <sys/select.h>

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "../HBGlobal.hpp"
#include "../HBContext.hpp"

HONEYBEE_BEGIN_NAMESPACE

// X11 related local variables
static Display *x_display = nullptr;
static Atom s_wmDeleteMessage;

class EglAdapter {
public:
    EglAdapter(HBContext *context);
    EGLBoolean createWindow(const char *title);
    void windowLoop();

private:
    void signalHandler(int signal);
private:
    HBContext *mContext;
    bool mDone = false;
};

HONEYBEE_END_NAMESPACE
