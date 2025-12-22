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
    static constexpr std::string name {"Block Shader"};

    Block(std::shared_ptr<textures::Loader> texLoader) : Shader(texLoader)
    {}

    errors::expected<> bindTextures();
    errors::expected<> unbindTextures();

    errors::expected<> draw(GLuint VAO, size_t size);

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
Block::bindTextures()
{
    if (!valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is not in a valid state in call to bindTextures", name), errors::Code::InvalidState);
    }

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
        return errors::unexpected(fmt::format("Error bind in shader [{}]: 0x{:x}", name, err), errors::Code::Unknown);
    }

    return {};
}

inline
errors::expected<>
Block::unbindTextures()
{
    if (!valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is not in a valid state in call to unbindTextures", name), errors::Code::InvalidState);
    }

    // Always good practice to set everything back to defaults once configured.
    for (GLuint i = 0; i < texs->size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected(fmt::format("Error unbind in shader [{}]: 0x{:x}", name, err), errors::Code::Unknown);
    }

    return {};
}

inline
errors::expected<>
Block::draw(GLuint VAO, size_t size)
{
    if (!valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is not in a valid state in call to draw", name), errors::Code::InvalidState);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, size);
    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected(fmt::format("Error draw in shader [{}]: 0x{:x}", name, err),  errors::Code::Unknown);
    }

    return {};
}

inline
errors::expected<>
Block::attachShaders() const
{
    if (valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is in a valid state in call to attachShaders", name), errors::Code::InvalidState);
    }

    Loader loader;
    
    errors::expected<GLuint> vert = loader.load("block/block.vert", GL_VERTEX_SHADER);
    if (errors::has_error(vert))
    {
        return errors::unexpected(vert.error());
    }
    
    errors::expected<GLuint> geom = loader.load("block/block.geom", GL_GEOMETRY_SHADER);
    if (errors::has_error(geom))
    {
        return errors::unexpected(geom.error());
    }
    
    errors::expected<GLuint> frag = loader.load("block/block.frag", GL_FRAGMENT_SHADER);
    if (errors::has_error(frag))
    {
        return errors::unexpected(frag.error());
    }

    // Attach all shaders once loading has successfuly completed
    glAttachShader(program, vert.value());
    glAttachShader(program, geom.value());
    glAttachShader(program, frag.value());

    errors::expected<> ret = linkProgram();
    
    // Clean up whether success or failure
    glDeleteShader(vert.value());
    glDeleteShader(geom.value());
    glDeleteShader(frag.value());
    
    return ret;
}

inline
errors::expected<>
Block::loadTextures(std::shared_ptr<textures::Loader> texLoader)
{
    if (valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is in a valid state in call to loadTextures", name), errors::Code::InvalidState);
    }

    Use();

    const std::string tmp {"blocks"};
    textures::TextureSet texSet = texLoader->getTextureSet(tmp);
    texs = texSet.textures;

    glUniform1i(glGetUniformLocation(getProgram(), "material.diffuse"), 0);
	GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("Failed to set material's diffuse: " + std::to_string(err), errors::Code::InitializationError);
    }

    glUniform1i(glGetUniformLocation(getProgram(), "material.specular"), 1);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("Failed to set material's specular: " + std::to_string(err), errors::Code::InitializationError);
    }

	int offsetArray[16] = { 0, 1, -1, 0, 1, 0, -1, 0, 1, -1, 0, 0, 0, 0, 0, 1 };
	glm::mat4 offsetTransform = glm::make_mat4(offsetArray);
	GLint offsetTransformLoc = glGetUniformLocation(getProgram(), "offsetTransform");
	glUniformMatrix4fv(offsetTransformLoc, 1, GL_FALSE, glm::value_ptr(offsetTransform));
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("Failed to set offset matrix: " + std::to_string(err), errors::Code::InitializationError);
    }

	// Set the tile dimensions for this shader
	glUniform2f(glGetUniformLocation(getProgram(), "texDim"), texSet.tileNumWidth, texSet.tileNumHeight);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("Failed to set texture dimensions: " + std::to_string(err), errors::Code::InitializationError);
    }

	// Directional Light
	GLint lightDirLoc = glGetUniformLocation(getProgram(), "dirLight.direction");
	glUniform3f(lightDirLoc, -10.0f, 10.0f, -10.0f);
	glUniform3f(glGetUniformLocation(getProgram(), "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(getProgram(), "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(getProgram(), "dirLight.specular"), 1.0f, 1.0f, 1.0f);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("Failed to set lighting settings: " + std::to_string(err), errors::Code::InitializationError);
    }

    return {};
}

} // namespace shaders