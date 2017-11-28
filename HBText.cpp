#include "HBText.hpp"
#include "HBHelper.hpp"
#include "freetype/ftmodapi.h"
#include "HBContext.hpp"

HONEYBEE_BEGIN_NAMESPACE

HBText::HBText(int fontSize, const HBContext *context) {
    mProgramObject = honeybee::HBHelper::loadProgramByPath("textVertexShader.vs", "textFragShader.fs");

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Could not init freetype library" << std::endl;
        exit(1);
    }

    FT_Face face;
    if (FT_New_Face(ft, "Fonts/Helvetica-Regular.ttf", 0, &face)) {
        std::cerr << "Could not open font" << std::endl;
        exit(1);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                    );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint)(face->glyph->advance.x)
        };
        mCharacters.insert(std::pair<GLchar, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_Library(ft);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint tex{0}, sampler{0};
    glGenVertexArrays(1, &mVaoId);
    glGenBuffers(1, &mVboId);
    glBindVertexArray(mVaoId);
    glGenTextures(1, &tex);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindSampler(0, sampler);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVboId);
    glBindVertexArray(0);

    glUseProgram(mProgramObject);
    glUniform1i(glGetUniformLocation(mProgramObject, "tex"), 0);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(context->width), 0.0f, static_cast<float>(context->height));
    glUniformMatrix4fv(glGetUniformLocation(mProgramObject, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void HBText::draw() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glUseProgram(mProgramObject);
    glUniform4fv(glGetUniformLocation(mProgramObject, "color"), 1, glm::value_ptr(mColor));
    glBindVertexArray(mVaoId);

    auto x = mX;
    auto y = mY;
    for (auto p : mText) {
        auto ch = mCharacters[p];
        GLfloat xpos = x + ch.Bearing.x * mScale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * mScale;

        GLfloat w = ch.Size.x * mScale;
        GLfloat h = ch.Size.y * mScale;
        GLfloat box[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), box, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * mScale;
    }
    glDisable(GL_BLEND);
}

void HBText::setText(const std::string &text) {
    mText = text;
}

void HBText::setPosition(const float &x, const float &y) {
    mX = x;
    mY = y;
}

void HBText::setColor(const glm::vec4 &color) {
    mColor = std::move(color);
}

HONEYBEE_END_NAMESPACE
