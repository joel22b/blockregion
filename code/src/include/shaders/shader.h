#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <optional>

#include <GL/glew.h>

namespace shaders
{

template <typename Derived>
class Shader
{
public:
    Shader();

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
};

/********************************
 * Definitions
********************************/

template <typename Derived>
inline
Shader<Derived>::Shader()
{
    program = glCreateProgram();
    static_cast<const Derived*>(this)->attachShaders();
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
