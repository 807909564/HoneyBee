#include <iostream>
#include "HBEgl.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "HBShader.hpp"

typedef struct {
    // Handle to a program object
    GLuint programObject;
    unsigned int tex1;
    unsigned int tex2;
} UserData;

int main() {
    honeybee::HBEgl egl;
    egl.glContext()->userData = malloc(sizeof(UserData));
    egl.createWindow("Hello EGL", 800, 700, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    egl.registerInitFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        stbi_set_flip_vertically_on_load(true);
        {
            int width, height, nrChannels;
            unsigned char *data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);

            glGenTextures(1, &(userData->tex1));
            glBindTexture(GL_TEXTURE_2D, userData->tex1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }

        {
            int width, height, nrChannels;
            unsigned char *data = stbi_load("graffiti.png", &width, &height, &nrChannels, 0);

            glGenTextures(1, &(userData->tex2));
            glBindTexture(GL_TEXTURE_2D, userData->tex2);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }

        char vShaderStr[] =
                "#version 300 es                          \n"
                "layout(location = 1) in vec3 vPosition;  \n"
                "layout(location = 2) in vec2 vTexCoord;  \n"
                "out vec2 texCoord;                       \n"
                "uniform mat4 model;                   \n"
                "uniform mat4 view;                   \n"
                "uniform mat4 projection;                   \n"
                "void main()                              \n"
                "{                                        \n"
                "   gl_Position = projection * view * model * vec4(vPosition, 1.0);              \n"
                "   texCoord = vTexCoord;                 \n"
                "}                                        \n";

        char fShaderStr[] =
                "#version 300 es                              \n"
                "precision mediump float;                     \n"
                "out vec4 fragColor;                          \n"
                "in vec2 texCoord;                          \n"
                "uniform sampler2D texture1;              \n"
                "uniform sampler2D texture2;              \n"
                "void main()                                  \n"
                "{                                            \n"
                "   fragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), texture(texture2, texCoord).a * 0.8);                  \n"
                "}                                            \n";

        // Load the vertex/fragment shaders
        auto programObject = honeybee::HBShader::loadProgram(vShaderStr, fShaderStr);
        if (programObject == 0) return false;

        // Store the program object
        userData->programObject = programObject;

        glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

        glUseProgram(userData->programObject);
        glUniform1i(glGetUniformLocation(userData->programObject, "texture1"), 0);
        glUniform1i(glGetUniformLocation(userData->programObject, "texture2"), 1);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)context->width / context->height, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        return true;
    });
    egl.registerDrawFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        unsigned int eboId;
        glGenBuffers(1, &eboId);

        unsigned int vaoId;
        glGenVertexArrays(1, &vaoId);
        unsigned int vboId;
        glGenBuffers(1, &vboId);

        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0 );
        glEnableVertexAttribArray ( 1 );

        glVertexAttribPointer ( 2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)) );
        glEnableVertexAttribArray ( 2 );

        // Set the viewport
        glViewport ( 0, 0, context->width, context->height );

        // Clear the color buffer
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Use the program object
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, userData->tex1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, userData->tex2);

        glUseProgram ( userData->programObject );
        glBindVertexArray(vaoId);

        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };
        for (unsigned int i = 0; i < 10; ++i) {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    });
    egl.registerUpdateFunc([](honeybee::HBContext *context, float deltaTime) {
        UserData *userData = (UserData *)(context->userData);
        static float time = 0;
        time += deltaTime;
        if (time > 360.0) {
            time = 0.0;
        }
        float radius = 10.0f;
        float camX = sin(time) * radius;
        float camZ = cos(time) * radius;
        glm::mat4 view;
        view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),
                   glm::vec3(0.0f, 0.0f, 0.0f),
                   glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "view"), 1, GL_FALSE, glm::value_ptr(view));
    });
    egl.registerShutdownFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        glDeleteProgram(userData->programObject);
    });
    return egl.exec();
}
