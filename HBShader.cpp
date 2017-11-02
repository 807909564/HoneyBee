#include <iostream>
#include "HBShader.hpp"

HONEYBEE_BEGIN_NAMESPACE

GLuint HBShader::loadProgram(const char *vertShaderSrc, const char *fragShaderSrc) {
    // Load the vertex/fragment shaders
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertShaderSrc);
    if (vertexShader == 0) return 0;

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
    if (fragmentShader == 0) {
        glDeleteShader ( vertexShader );
        return 0;
    }

    // Create the program object
    GLuint programObject = glCreateProgram();
    if (programObject == 0) return 0;

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    GLint linked = 0;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;

        glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

        if (infoLen > 1) {
            auto infoLog = (char *)malloc(sizeof(char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, nullptr, infoLog);
            std::cout << "Error linking program: " << infoLog << std::endl;

            free(infoLog);
        }

        glDeleteProgram(programObject);
        return 0;
    }

    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

GLuint HBShader::loadShader(GLenum type, const char *shaderSrc) {
    // Create the shader object
    GLuint shader = glCreateShader(type);

    if (shader == 0) return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            auto infoLog = (char *)malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            std::cout << "Error compiling shader: " << infoLog << std::endl;

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

HONEYBEE_END_NAMESPACE
