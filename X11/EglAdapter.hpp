#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
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
    GLboolean userInterrupt();
private:
    HBContext *mContext;
};

HONEYBEE_END_NAMESPACE
