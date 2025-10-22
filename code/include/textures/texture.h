#pragma once

#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>

namespace textures
{

enum TextureType
{
    Unknown = 0,
    Error,
    Diffuse,
    Specular,
    Text
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

struct TextureSet
{
    std::shared_ptr<std::vector<Texture>> textures {};
    int tileWidth {0};
    int tileHeight {0};
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
            os << "unknown";
            break;
        case TextureType::Error:
            os << "error";
            break;
        case TextureType::Diffuse:
            os << "diffuse";
            break;
        case TextureType::Specular:
            os << "specular";
            break;
        case TextureType::Text:
            os << "text";
            break;
        default:
            os << "undefined";
            break;
    }
    return os;
}

inline
TextureType
toTextureType(const std::string& type)
{
    if (type == "error")
    {
        return TextureType::Error;
    }
    else if (type == "diffuse")
    {
        return TextureType::Diffuse;
    }
    else if (type == "specular")
    {
        return TextureType::Specular;
    }
    else if (type == "text")
    {
        return TextureType::Text;
    }
    return TextureType::Unknown;
}

} // namespace textures
