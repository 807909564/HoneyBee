#pragma once

#include <GLES3/gl3.h>
#include "HBGlobal.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

HONEYBEE_BEGIN_NAMESPACE

class HBText {
public:
    explicit HBText();
    ~HBText();
    void draw();

private:
    GLuint mProgramObject{0};
    GLuint mVboId{0};
    GLuint mVaoId{0};
    FT_Library mFt{nullptr};
    FT_Face mFace{nullptr};
    float mX{-0.5};
    float mY{0.0};
    float mScaleX{1.0 / 400.0};
    float mScaleY{1.0 / 350.0};
};

HONEYBEE_END_NAMESPACE
