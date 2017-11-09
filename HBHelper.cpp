#include <iostream>
// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "HBHelper.hpp"

HONEYBEE_BEGIN_NAMESPACE

GLuint HBHelper::loadProgram(const char *vertShaderSrc, const char *fragShaderSrc) {
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

GLuint HBHelper::loadProgramByPath(const char *vertShaderPath, const char *fragShaderPath) {
    int vLen = 0;
    int fLen = 0;
    int vFd = 0;
    int fFd = 0;
    auto vertShaderSrc = mapFile(vertShaderPath, vLen, vFd);
    auto fragShaderSrc = mapFile(fragShaderPath, fLen, fFd);
    int result = loadProgram(vertShaderSrc, fragShaderSrc);
    unmapFile(vFd, vertShaderSrc, vLen);
    unmapFile(fFd, fragShaderSrc, fLen);
    return result;
}

GLuint HBHelper::loadTextureFromFile(const std::string &fileName) {
 //   stbi_set_flip_vertically_on_load(true);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    auto data = stbi_load(fileName.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << fileName << std::endl;
    }
    return textureID;
}

GLuint HBHelper::loadShader(GLenum type, const char *shaderSrc) {
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

char *HBHelper::mapFile(const char *fileName, int &length, int &fd) {
    fd = open(fileName, O_RDONLY);
    if (fd == -1) {
        std::perror("Open file failed!");
        exit(255);
    }

    // obtain file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::perror("Failed to get file size!");
        exit(255);
    }

    length = sb.st_size;

    auto addr = static_cast<char *>(mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0u));
    if (addr == MAP_FAILED) {
        std::perror("Memory map failed!");
        exit(255);
    }
    return addr;
}

void HBHelper::unmapFile(int fd, char *addr, size_t len) {
    munmap(addr, len);
    close(fd);
}

HONEYBEE_END_NAMESPACE
