#include "EglAdapter.hpp"

HONEYBEE_BEGIN_NAMESPACE

EglAdapter::EglAdapter(HBContext *context) : mContext(context) {}

EGLBoolean EglAdapter::createWindow(const char *title) {
    x_display = XOpenDisplay(nullptr);
    if (x_display == nullptr) return EGL_FALSE;

    auto root = DefaultRootWindow(x_display);

    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    auto win = XCreateWindow(
                x_display, root,
                0, 0, mContext->width, mContext->height, 0,
                CopyFromParent, InputOutput,
                CopyFromParent, CWEventMask,
                &swa);
    s_wmDeleteMessage = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x_display, win, &s_wmDeleteMessage, 1);

    XSetWindowAttributes xattr;
    xattr.override_redirect = False;
    XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow(x_display, win);
    XStoreName(x_display, win, title);

    // get identifiers for the provided atom name strings
    Atom wm_state;
    wm_state = XInternAtom(x_display, "_NET_WM_STATE", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = False;
    XSendEvent (
                x_display,
                DefaultRootWindow(x_display),
                False,
                SubstructureNotifyMask,
                &xev );

    mContext->eglNativeWindow = (EGLNativeWindowType)win;
    mContext->eglNativeDisplay = (EGLNativeDisplayType)x_display;
    return EGL_TRUE;
}

void EglAdapter::windowLoop() {
    struct timeval t1, t2;
    struct timezone tz;
    float deltatime;

    gettimeofday(&t1, &tz);

    while (!userInterrupt()) {
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

GLboolean EglAdapter::userInterrupt() {
    XEvent xev;
    GLboolean userinterrupt = GL_FALSE;

    // Pump all messages from X server. Keypresses are directed to keyfunc (if defined)
    while (XPending(x_display)) {
        XNextEvent(x_display, &xev);
        if (xev.type == ClientMessage) {
            if (xev.xclient.data.l[0] == s_wmDeleteMessage) {
                userinterrupt = GL_TRUE;
            }
        }
        if (xev.type == DestroyNotify) userinterrupt = GL_TRUE;
    }
    return userinterrupt;
}

HONEYBEE_END_NAMESPACE
