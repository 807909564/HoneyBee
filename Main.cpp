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
    GLuint lightProgramObject;
    unsigned int tex1;
    unsigned int tex2;
} UserData;

int main() {
    honeybee::HBEgl egl;
    egl.glContext()->userData = malloc(sizeof(UserData));
    egl.createWindow("Hello EGL", 800, 700, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    egl.registerInitFunc([](honeybee::HBContext *context) {
        // Load the vertex/fragment shaders
        auto programObject = honeybee::HBShader::loadProgramByPath("vertexShader.vs", "fragShader.fs");
        if (programObject == 0) return false;
        auto lightProgramObject = honeybee::HBShader::loadProgramByPath("lightVertexShader.vs", "lightFragShader.fs");
        if (lightProgramObject == 0) {
            glDeleteProgram(programObject);
            return false;
        }

        // Store the program object
        UserData *userData = (UserData *)(context->userData);
        userData->programObject = programObject;
        userData->lightProgramObject = lightProgramObject;

        glm::mat4 view;
        glm::vec3 viewPos(4.0f, 2.0f, 5.0f);
        view = glm::lookAt(viewPos,
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)context->width / context->height, 0.1f, 100.0f);

        glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

        {
            glUseProgram(userData->programObject);
            glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
            glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
            glUniform3fv(glGetUniformLocation(userData->programObject, "objectColor"), 1, glm::value_ptr(objectColor));
            glUniform3fv(glGetUniformLocation(userData->programObject, "lightColor"), 1, glm::value_ptr(lightColor));
            glUniform3fv(glGetUniformLocation(userData->programObject, "viewPos"), 1, glm::value_ptr(viewPos));

            glm::mat4 model;
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(userData->programObject, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        }

        {
            glUseProgram(userData->lightProgramObject);
            glUniformMatrix4fv(glGetUniformLocation(userData->lightProgramObject, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(userData->lightProgramObject, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        }

        return true;
    });
    egl.registerDrawFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };

        unsigned int vboId;
        glGenBuffers(1, &vboId);

        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        unsigned int vaoId;
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);

        unsigned int lightVaoId;
        glGenVertexArrays(1, &lightVaoId);
        glBindVertexArray(lightVaoId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // Set the viewport
        glViewport ( 0, 0, context->width, context->height );

        // Clear the color buffer
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw lamb
        {
            glUseProgram(userData->lightProgramObject);
            glBindVertexArray(lightVaoId);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // draw container
        {
            glUseProgram(userData->programObject);
            glBindVertexArray(vaoId);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    });
    egl.registerUpdateFunc([](honeybee::HBContext *context, float deltaTime) {
        UserData *userData = (UserData *)(context->userData);
        static float time = 0.0;
        time += deltaTime * 2.0;
        if (time > 360.0) {
            time = 0.0;
        }

        float radius = 2.0f;
        float lightX = sin(time) * radius;
        float lightZ = cos(time) * radius;

        glm::vec3 lightPos(lightX, 1.0f, lightZ);
        glUseProgram(userData->lightProgramObject);
        glm::mat4 model;
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        glUniformMatrix4fv(glGetUniformLocation(userData->lightProgramObject, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glUseProgram(userData->programObject);
        glUniform3fv(glGetUniformLocation(userData->programObject, "lightPos"), 1, glm::value_ptr(lightPos));
    });
    egl.registerShutdownFunc([](honeybee::HBContext *context) {
        UserData *userData = (UserData *)(context->userData);
        glDeleteProgram(userData->programObject);
        glDeleteProgram(userData->lightProgramObject);
    });
    return egl.exec();
}
