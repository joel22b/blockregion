#pragma once

#include "shaders/loader.h"
#include "shaders/shader.h"

namespace shaders
{

class Block : public Shader<Block>
{
public:
    const std::string_view name {"Block Shader"};

    bool attachShaders() const;

private:
};

/********************************
 * Definitions
********************************/

inline
bool
Block::attachShaders() const
{
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
    
    return ret;
}

} // namespace shaders