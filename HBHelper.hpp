#pragma once

#include <stdlib.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "HBGlobal.hpp"

HONEYBEE_BEGIN_NAMESPACE

class HBHelper {
public:
    static GLuint loadProgram(const char *vertShaderSrc, const char *fragShaderSrc);
    static GLuint loadProgramByPath(const char *vertShaderPath, const char *fragShaderPath);
    static GLuint loadTextureFromFile(const std::string &fileName);
private:
    static GLuint loadShader(GLenum type, const char *shaderSrc);
    static char *mapFile(const char *fileName, int &length, int &fd);
    static void unmapFile(int fd, char *addr, size_t len);
};

HONEYBEE_END_NAMESPACE
