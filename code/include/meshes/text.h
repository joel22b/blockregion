#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shaders/text.h>

namespace meshes
{

class Text
{
public:
    Text(std::shared_ptr<shaders::Text> textShader, std::string text, int x, int y, float scale, glm::vec3 color) :
        textShader(textShader), text(text), x(x), y(y), scale(scale), color(color)
    {
        textShader->Use();

        projection = glm::ortho(0.0f, 1600.0f, 0.0f, 800.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };
    ~Text() {};

    void render();

private:
    std::shared_ptr<shaders::Text> textShader;
    GLuint VAO, VBO;
    glm::mat4 projection;

    std::string text;
    int x;
    int y;
    float scale;
    glm::vec3 color;
};

inline
void
Text::render()
{
    if (!textShader)
    {
        std::cout << "TEXT SHADER DOESN'T EXIST" << std::endl;
        //return;
    }

    textShader->bindTextures(color);
    textShader->draw(VAO, VBO, text, x, y, scale);
    textShader->unbindTextures();
}

}; // namespace meshes
