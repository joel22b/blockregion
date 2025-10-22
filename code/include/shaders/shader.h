#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <optional>
#include <vector>
#include <memory>

#include <GL/glew.h>

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
    bool linkProgram() const;

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
        std::cout << "Error shader start: " << err << std::endl;
    }

    program = glCreateProgram();
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error shader program: " << err << std::endl;
    }

    static_cast<const Derived*>(this)->attachShaders();

    static_cast<Derived*>(this)->loadTextures(texLoader);
}

template <typename Derived>
inline
bool
Shader<Derived>::linkProgram() const
{
    glLinkProgram(program);
    // Print linking errors if any
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR: Shader failed to link program: " << static_cast<const Derived*>(this)->name << std::endl << infoLog << std::endl;
        return false;
    }

    return true;
}

} // namespace shaders
