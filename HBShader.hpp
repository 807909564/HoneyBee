#pragma once

#include <stdlib.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "HBGlobal.hpp"

HONEYBEE_BEGIN_NAMESPACE

class HBShader {
public:
    static GLuint loadProgram(const char *vertShaderSrc, const char *fragShaderSrc);
private:
    static GLuint loadShader(GLenum type, const char *shaderSrc);
};

HONEYBEE_END_NAMESPACE
