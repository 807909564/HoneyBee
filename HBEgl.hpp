#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "HBContext.hpp"
#include "EglAdapter.hpp"
#include "HBGlobal.hpp"

HONEYBEE_BEGIN_NAMESPACE

#define INVERTED_BIT            (1 << 5)
#pragma pack(push,x1)                            // Byte alignment (8-bit)
#pragma pack(1)

typedef struct {
   unsigned char  IdSize,
            MapType,
            ImageType;
   unsigned short PaletteStart,
            PaletteSize;
   unsigned char  PaletteEntryDepth;
   unsigned short X,
            Y,
            Width,
            Height;
   unsigned char  ColorDepth,
            Descriptor;

} TGA_HEADER;

#pragma pack(pop,x1)

class HBEgl final {
public:
    explicit HBEgl();
    ~HBEgl();
    HBContext *glContext() const;
    GLboolean createWindow(const char *title, GLint width, GLint height, GLuint flags);
    void registerDrawFunc(std::function<void(HBContext *)> drawCallback);
    void registerUpdateFunc(std::function<void(HBContext *, float)> updateCallback);
    void registerShutdownFunc(std::function<void(HBContext *)> shutdownCallback);
    void registerInitFunc(std::function<bool(HBContext *)> initCallback);

    int exec();

private:
    EGLint getContextRenderableType(EGLDisplay eglDisplay);
private:
    HBContext *mContext{nullptr};
    std::unique_ptr<EglAdapter> mEglAdapter;
    std::function<bool(HBContext *)> mInitFunc;
};

HONEYBEE_END_NAMESPACE
