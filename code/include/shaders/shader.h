#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <optional>
#include <vector>
#include <memory>

#include <GL/glew.h>

#include <errors/br-expected.h>

#include <textures/loader.h>

namespace shaders
{

template <typename Derived>
class Shader
{
public:
    Shader(std::shared_ptr<textures::Loader> texLoader);

    void Use()
    {
        glUseProgram(program);
    }

    GLuint getProgram()
    {
        return program;
    }

protected:
    errors::expected<> linkProgram() const;

    GLuint program;

    std::shared_ptr<std::vector<textures::Texture>> texs;
};

/********************************
 * Definitions
********************************/

template <typename Derived>
inline
Shader<Derived>::Shader(std::shared_ptr<textures::Loader> texLoader)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::string derivedName {static_cast<const Derived*>(this)->name};
        std::cout << "Error shader start: " << derivedName << " Error: " << err << std::endl;
    }

    program = glCreateProgram();
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error shader program: " << err << std::endl;
    }

    errors::expected<> retAttach = static_cast<const Derived*>(this)->attachShaders();
    if (errors::has_error(retAttach))
    {
        std::cout << "Failed to attach shaders: " << retAttach.error() << std::endl;
        return;
    }

    errors::expected<> retTextures = static_cast<Derived*>(this)->loadTextures(texLoader);
    if (errors::has_error(retTextures))
    {
        std::cout << "Failed to load textures: " << retTextures.error() << std::endl;
        return;
    }
}

template <typename Derived>
inline
errors::expected<>
Shader<Derived>::linkProgram() const
{
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::string errMsg {"ERROR: Shader failed to link program: "};
        errMsg += static_cast<const Derived*>(this)->name;
        errMsg += "\n\r";
        errMsg += infoLog;
        return errors::unexpected(errMsg, errors::Code::InitializationError);
    }

    return {};
}

} // namespace shaders
