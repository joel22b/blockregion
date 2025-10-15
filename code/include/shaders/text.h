#pragma once

#include <string>
#include <map>

#include "shaders/loader.h"
#include "shaders/shader.h"

#include "glm/glm.hpp"

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

namespace shaders
{

class Text : public Shader<Text>
{
public:
    const std::string_view name {"Text Shader"};

    Text(std::shared_ptr<textures::Loader> texLoader) : Shader(texLoader)
    {}

    void bindTextures(glm::vec3 color);
    void unbindTextures();

    void draw(GLuint VAO, GLuint VBO, std::map<char, Character> Characters, std::string text, float x, float y, float scale);

    // This should only be called by Shader class
    bool attachShaders() const;
    void loadTextures(std::shared_ptr<textures::Loader> texLoader);

private:
};

/********************************
 * Definitions
********************************/

inline
void
Text::bindTextures(glm::vec3 color)
{
    Use();

    glUniform3f(glGetUniformLocation(getProgram(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
}

inline
void
Text::unbindTextures()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline
void
Text::draw(GLuint VAO, GLuint VBO, std::map<char, Character> Characters, std::string text, float x, float y, float scale)
{
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
}

inline
bool
Text::attachShaders() const
{
    Loader loader;
    
    std::optional<GLuint> vert = loader.load("text/text.vert", GL_VERTEX_SHADER);
    if (!vert.has_value())
    {
        return false;
    }
    glAttachShader(program, vert.value());
    
    std::optional<GLuint> frag = loader.load("text/text.frag", GL_FRAGMENT_SHADER);
    if (!frag.has_value())
    {
        return false;
    }
    glAttachShader(program, frag.value());

    bool ret = linkProgram();
    
    glDeleteShader(vert.value());
    glDeleteShader(frag.value());
    
    return ret;
}

inline
void
Text::loadTextures(std::shared_ptr<textures::Loader> texLoader)
{}

} // namespace shaders