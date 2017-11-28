#pragma once

#include <map>
#include <GLES3/gl3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "HBGlobal.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

HONEYBEE_BEGIN_NAMESPACE

class HBContext;

class HBText final {
public:
    explicit HBText(int fontSize, const HBContext *context);
    void draw();

    void setText(const std::string &text);
    void setPosition(const float &x, const float &y);
    void setColor(const glm::vec4 &color);

private:
    GLuint mProgramObject{0};
    GLuint mVboId{0};
    GLuint mVaoId{0};
    float mX{0.0};
    float mY{0.0};
    float mScale{1.0};
    glm::vec4 mColor{glm::vec4(1.0, 1.0, 1.0, 1.0)};

    struct Character {
        GLuint     TextureID;  // ID handle of the glyph texture
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        GLuint     Advance;    // Offset to advance to next glyph
    };
    std::map<GLchar, Character> mCharacters;

    std::string mText;
};

HONEYBEE_END_NAMESPACE
