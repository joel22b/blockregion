#pragma once

#include <iostream>
#include <string>
#include <map>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shaders/text.h"

/*struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};*/

class Text {
public:
	Text(const char *path, int fontSize = 48)
    {
        this->fontSize = fontSize;

        std::shared_ptr<textures::Loader> texLoader = nullptr;
        textShader = std::make_unique<shaders::Text>(texLoader);

        //textShader = shaders::Shader<shaders::Text>("/home/jbraun/projects/blockregion/code/shaders/text.vert", "/home/jbraun/projects/blockregion/code/shaders/text.frag");
        textShader->Use();

        FT_Library ft;
        auto errorFtInit = FT_Init_FreeType(&ft);
        if (errorFtInit)
        {
            std::cout << "ERROR::FREETYPE::Could_not_init_FreeType_Library: " << errorFtInit << std::endl;
        }

        FT_Face face;
        auto errorFaceNew = FT_New_Face(ft, path, 0, &face);
        if (errorFaceNew)
        {
            std::cout << "ERROR::FREETYPE::Failed_to_load_font: " << errorFaceNew << std::endl;
            std::cout << "\t" << path << std::endl;
        }
        else {
            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, fontSize);

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph 
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
                unsigned int texture;
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
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                Characters.insert(std::pair<char, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

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
    }

    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
    {
        if (!textShader)
        {
            std::cout << "TEXT SHADER DOESN'T EXIST" << std::endl;
            //return;
        }

        textShader->bindTextures(color);
        textShader->draw(VAO, VBO, Characters, text, x, y, scale);
        textShader->unbindTextures();
    }

private:
    int fontSize;
    std::map<char, Character> Characters;
    glm::mat4 projection;
    unsigned int VAO, VBO;
    std::unique_ptr<shaders::Text> textShader;
};
