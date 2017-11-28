#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "HBText.hpp"
#include "HBHelper.hpp"
#include "freetype/ftmodapi.h"

HONEYBEE_BEGIN_NAMESPACE

HBText::HBText() {
    mProgramObject = honeybee::HBHelper::loadProgramByPath("textVertexShader.vs", "textFragShader.fs");

    if (FT_Init_FreeType(&mFt)) {
        std::cerr << "Could not init freetype library" << std::endl;
        exit(1);
    }

    if (FT_New_Face(mFt, "./Helvetica-Regular.ttf", 0, &mFace)) {
        std::cerr << "Could not open font" << std::endl;
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 700.0f);

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
    glUniform4fv(glGetUniformLocation(mProgramObject, "color"), 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
    glUniform4fv(glGetUniformLocation(mProgramObject, "projection"), 1, glm::value_ptr(projection));
}

HBText::~HBText() {
    FT_Done_Face(mFace);
    FT_Done_Library(mFt);
}

void HBText::draw() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glUseProgram(mProgramObject);
    glBindVertexArray(mVaoId);

    auto g = mFace->glyph;

    FT_Set_Pixel_Sizes(mFace, 0, 64);

    //for (auto p = "Hello World!"; *p; ++p) {
        if (FT_Load_Char(mFace, 'X', FT_LOAD_RENDER)) exit(1);
        glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    g->bitmap.width,
                    g->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    g->bitmap.buffer
                    );

        const float vx = mX + g->bitmap_left * mScaleX;
        const float vy = mY + g->bitmap_top * mScaleY;
        const float w = g->bitmap.width * mScaleX;
        const float h = g->bitmap.rows * mScaleY;
        GLfloat box[6][4] = {
            {vx    , vy    , 0, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy    , 1, 0},
            {vx + w, vy    , 1, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy - h, 1, 1}
        };

        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), box, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

//        mX += (g->advance.x << 6) * mScaleX;
//        mY += (g->advance.y << 6) * mScaleY;
    //}

  //  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

HONEYBEE_END_NAMESPACE
