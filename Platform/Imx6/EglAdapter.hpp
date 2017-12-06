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

#include "../../HBGlobal.hpp"
#include "../../HBContext.hpp"

HONEYBEE_BEGIN_NAMESPACE

class EglAdapter {
public:
    EglAdapter(HBContext *context);
    EGLBoolean createWindow(const char *title);
    void windowLoop();

private:
    HBContext *mContext{nullptr};
};

HONEYBEE_END_NAMESPACE
