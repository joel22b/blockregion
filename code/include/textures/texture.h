#pragma once

#include <string>

#include <GL/glew.h>

namespace textures
{

enum TextureType
{
    Unknown = 0,
    Error,
    Defuse,
    Specular
};

struct Texture
{
    GLuint id;
    TextureType type {TextureType::Unknown};
};

struct Tile
{
    int totalWidth {0};
    int totalHeight {0};
    int numWidth {0};
    int numHeight {0};
};

/********************************
 * Definitions
********************************/

inline
std::ostream&
operator<<(std::ostream& os, const TextureType& type)
{
    switch (type)
    {
        case TextureType::Unknown:
            os << "Unknown";
            break;
        case TextureType::Error:
            os << "Error";
            break;
        case TextureType::Defuse:
            os << "texture_diffuse";
            break;
        case TextureType::Specular:
            os << "texture_specular";
            break;
        default:
            os << "Undefined";
            break;
    }
    return os;
}

} // namespace textures
