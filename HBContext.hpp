#pragma once

#include <stdlib.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "HBGlobal.hpp"

HONEYBEE_BEGIN_NAMESPACE

/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1
/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2
/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8

struct HBContext {
   /// Put platform specific data here
   void       *platformData;

   /// Put your user data here...
   void       *userData;

   /// Window width
   GLint       width;

   /// Window height
   GLint       height;

   /// Display handle
   EGLNativeDisplayType eglNativeDisplay;

   /// Window handle
   EGLNativeWindowType  eglNativeWindow;

   /// EGL display
   EGLDisplay  eglDisplay;

   /// EGL context
   EGLContext  eglContext;

   /// EGL surface
   EGLSurface  eglSurface;

   /// Callbacks
   std::function<void(HBContext *)> drawFunc;
   std::function<void(HBContext *)> shutdownFunc;
   std::function<void(HBContext *, float deltaTime)> updateFunc;
};

HONEYBEE_END_NAMESPACE
