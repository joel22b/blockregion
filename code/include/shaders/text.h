#pragma once

#include <string>
#include <vector>
#include <memory>

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
    static constexpr std::string name {"Text Shader"};

    Text(std::shared_ptr<textures::Loader> texLoader) : Shader(texLoader)
    {}

    errors::expected<> bindTextures(glm::vec3 color);
    errors::expected<> unbindTextures();

    errors::expected<> draw(GLuint VAO, GLuint VBO, std::string text, float x, float y, float scale);

    // This should only be called by Shader class
    errors::expected<> attachShaders() const;
    errors::expected<> loadTextures(std::shared_ptr<textures::Loader> texLoader);

private:
};

/********************************
 * Definitions
********************************/

inline
errors::expected<>
Text::bindTextures(glm::vec3 color)
{
    if (!valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is not in a valid state in call to bindTextures", name), errors::Code::InvalidState);
    }

    Use();

    glUniform3f(glGetUniformLocation(getProgram(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    return {};
}

inline
errors::expected<>
Text::unbindTextures()
{
    if (!valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is not in a valid state in call to unbindTextures", name), errors::Code::InvalidState);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return {};
}

inline
errors::expected<>
Text::draw(GLuint VAO, GLuint VBO, std::string text, float x, float y, float scale)
{
    if (!valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is not in a valid state in call to draw", name), errors::Code::InvalidState);
    }

    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        textures::Texture ch = texs->at(*c);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
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
        glBindTexture(GL_TEXTURE_2D, ch.id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);

    return {};
}

inline
errors::expected<>
Text::attachShaders() const
{
    if (valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is in a valid state in call to attachShaders", name), errors::Code::InvalidState);
    }

    Loader loader;
    
    errors::expected<GLuint> vert = loader.load("text/text.vert", GL_VERTEX_SHADER);
    if (errors::has_error(vert))
    {
        return errors::unexpected(vert.error());
    }
    
    errors::expected<GLuint> frag = loader.load("text/text.frag", GL_FRAGMENT_SHADER);
    if (errors::has_error(frag))
    {
        return errors::unexpected(frag.error());
    }

    // Attach all shaders once loading has successfuly completed
    glAttachShader(program, vert.value());
    glAttachShader(program, frag.value());

    errors::expected<> ret = linkProgram();
    
    // Clean up whether success or failure
    glDeleteShader(vert.value());
    glDeleteShader(frag.value());
    
    return ret;
}

inline
errors::expected<>
Text::loadTextures(std::shared_ptr<textures::Loader> texLoader)
{
    if (valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is in a valid state in call to loadTextures", name), errors::Code::InvalidState);
    }

    Use();

    textures::TextureSet texSet = texLoader->getTextureSet("arial");
    texs = texSet.textures;

    return {};
}

} // namespace shaders
