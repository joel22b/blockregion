#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "shaders/loader.h"
#include "shaders/shader.h"

#include "textures/loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace shaders
{

class Block : public Shader<Block>
{
public:
    const std::string_view name {"Block Shader"};

    Block(std::shared_ptr<textures::Loader> texLoader) : Shader(texLoader)
    {}

    void bindTextures();
    void unbindTextures();

    void draw(GLuint VAO, size_t size);

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
Block::bindTextures()
{
    //std::cout << "bind loadTextures size: " << texs->size() << std::endl;

    Use();

    // Bind appropriate textures
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;

    for (GLuint i = 0; i < texs->size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
        // Retrieve texture number (the N in diffuse_textureN)
        std::stringstream ss;
        ss << texs->at(i).type;

        if (texs->at(i).type == textures::TextureType::Diffuse)
        {
            ss << diffuseNr++; // Transfer GLuint to stream
        }
        else if (texs->at(i).type == textures::TextureType::Specular)
        {
            ss << specularNr++; // Transfer GLuint to stream
        }

        std::string name = ss.str();
        // Now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(getProgram(), name.c_str()), i);
        // And finally bind the texture
        glBindTexture(GL_TEXTURE_2D, texs->at(i).id);
    }

    // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
    glUniform1f(glGetUniformLocation(getProgram(), "material.shininess"), 32.0f);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error bind: " << err << std::endl;
    }
}

inline
void
Block::unbindTextures()
{
    // Always good practice to set everything back to defaults once configured.
    for (GLuint i = 0; i < texs->size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error unbind: " << err << std::endl;
    }
}

inline
void
Block::draw(GLuint VAO, size_t size)
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, size);
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error draw: " << err << std::endl;
    }
}

inline
bool
Block::attachShaders() const
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error attach start: " << err << std::endl;
    }

    Loader loader;
    
    std::optional<GLuint> vert = loader.load("block/block.vert", GL_VERTEX_SHADER);
    if (!vert.has_value())
    {
        return false;
    }
    glAttachShader(program, vert.value());
    
    std::optional<GLuint> geom = loader.load("block/block.geom", GL_GEOMETRY_SHADER);
    if (!geom.has_value())
    {
        return false;
    }
    glAttachShader(program, geom.value());
    
    std::optional<GLuint> frag = loader.load("block/block.frag", GL_FRAGMENT_SHADER);
    if (!frag.has_value())
    {
        return false;
    }
    glAttachShader(program, frag.value());

    bool ret = linkProgram();
    
    glDeleteShader(vert.value());
    glDeleteShader(geom.value());
    glDeleteShader(frag.value());

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error attach end: " << err << std::endl;
    }
    
    return ret;
}

inline
void
Block::loadTextures(std::shared_ptr<textures::Loader> texLoader)
{
    Use();

    const std::string tmp {"blocks"};
    textures::TextureSet texSet = texLoader->getTextureSet(tmp);
    texs = texSet.textures;
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error load start: " << err << std::endl;
    }

    glUniform1i(glGetUniformLocation(getProgram(), "material.diffuse"), 0);
	err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error load mats diffuse: " << err << std::endl;
    }

    glUniform1i(glGetUniformLocation(getProgram(), "material.specular"), 1);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error load mats specular: " << err << std::endl;
    }

	int offsetArray[16] = { 0, 1, -1, 0, 1, 0, -1, 0, 1, -1, 0, 0, 0, 0, 0, 1 };
	glm::mat4 offsetTransform = glm::make_mat4(offsetArray);
	GLint offsetTransformLoc = glGetUniformLocation(getProgram(), "offsetTransform");
	glUniformMatrix4fv(offsetTransformLoc, 1, GL_FALSE, glm::value_ptr(offsetTransform));
	
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error load offset: " << err << std::endl;
    }

	// Set the tile dimensions for this shader
	glUniform2f(glGetUniformLocation(getProgram(), "texDim"), texSet.tileWidth, texSet.tileHeight);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error load tile: " << err << std::endl;
    }

	// Directional Light
	GLint lightDirLoc = glGetUniformLocation(getProgram(), "dirLight.direction");
	glUniform3f(lightDirLoc, -10.0f, 10.0f, -10.0f);
	glUniform3f(glGetUniformLocation(getProgram(), "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(getProgram(), "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(getProgram(), "dirLight.specular"), 1.0f, 1.0f, 1.0f);

    std::cout << "end loadTextures size: " << texs->size() << std::endl;

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error load end: " << err << std::endl;
    }
}

} // namespace shaders