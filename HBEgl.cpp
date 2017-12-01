#include "HBEgl.hpp"

HONEYBEE_BEGIN_NAMESPACE

HBEgl::HBEgl() {
    mContext = new HBContext;
    memset(mContext, 0, sizeof(mContext));
    mEglAdapter = make_unique<EglAdapter>(mContext);
}

HBEgl::~HBEgl() {
    if (mContext) {
        delete mContext;
        mContext = nullptr;
    }
}

HBContext *HBEgl::glContext() const {
    return mContext;
}

GLboolean HBEgl::createWindow(const char *title, GLint width, GLint height, GLuint flags) {
    if (mContext == nullptr) return GL_FALSE;
    mContext->width = width;
    mContext->height = height;

    if (!mEglAdapter->createWindow(title)) return GL_FALSE;

    mContext->eglDisplay = eglGetDisplay(mContext->eglNativeDisplay);
    if (mContext->eglDisplay == EGL_NO_DISPLAY) return GL_FALSE;

    // Initialize EGL
    EGLint majorVersion;
    EGLint minorVersion;
    if (!eglInitialize(mContext->eglDisplay, &majorVersion, &minorVersion)) return GL_FALSE;

    EGLint attribList[] =
    {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
        // if EGL_KHR_create_context extension is supported, then we will use
        // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
        EGL_RENDERABLE_TYPE, getContextRenderableType(mContext->eglDisplay),
        EGL_NONE
    };

    // Choose config
    EGLConfig config;
    EGLint numConfigs = 0;
    if (!eglChooseConfig(mContext->eglDisplay, attribList, &config, 1, &numConfigs)) return GL_FALSE;
    if (numConfigs < 1) return GL_FALSE;

    // Create a surface
    mContext->eglSurface = eglCreateWindowSurface(mContext->eglDisplay, config,
                                                  mContext->eglNativeWindow, nullptr);
    if (mContext->eglSurface == EGL_NO_SURFACE) return GL_FALSE;
    // Create a GL context
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    mContext->eglContext = eglCreateContext(mContext->eglDisplay, config,
                                            EGL_NO_CONTEXT, contextAttribs);

    if (mContext->eglContext == EGL_NO_CONTEXT) return GL_FALSE;

    // Make the context current
    if (!eglMakeCurrent(mContext->eglDisplay, mContext->eglSurface,
                        mContext->eglSurface, mContext->eglContext)) return GL_FALSE;

    return GL_TRUE;
}

void HBEgl::registerDrawFunc(std::function<void (HBContext *)> drawCallback) {
    mContext->drawFunc = drawCallback;
}

void HBEgl::registerUpdateFunc(std::function<void (HBContext *, float)> updateCallback) {
    mContext->updateFunc = updateCallback;
}

void HBEgl::registerShutdownFunc(std::function<void (HBContext *)> shutdownCallback) {
    mContext->shutdownFunc = shutdownCallback;
}

void HBEgl::registerInitFunc(std::function<bool(HBContext *)> initCallback) {
    mInitFunc = initCallback;
}

int HBEgl::exec() {
    if (mInitFunc) {
        if (!mInitFunc(mContext)) return 0;
    }
    mEglAdapter->windowLoop();
    if (mContext->shutdownFunc) {
        mContext->shutdownFunc(mContext);
    }
    return 0;
}

EGLint HBEgl::getContextRenderableType(EGLDisplay eglDisplay) {
#ifdef EGL_KHR_create_context
   auto extensions = eglQueryString(eglDisplay, EGL_EXTENSIONS);
   if (extensions != nullptr && strstr(extensions, "EGL_KHR_create_context")) {
      return EGL_OPENGL_ES3_BIT_KHR;
   }
#endif
   return EGL_OPENGL_ES2_BIT;
}

HONEYBEE_END_NAMESPACE
