#pragma once

#include "shaders/loader.h"
#include "shaders/shader.h"

namespace shaders
{

class Text : public Shader<Text>
{
public:
    const std::string_view name {"Text Shader"};

    bool attachShaders() const;

private:
};

/********************************
 * Definitions
********************************/

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

} // namespace shaders