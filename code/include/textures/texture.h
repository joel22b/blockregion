#pragma once

#include <string>
#include <ostream>
#include <sstream>
#include <vector>
#include <memory>
#include <map>

#include <GL/glew.h>

#include "glm/glm.hpp"

namespace textures
{

using TileID = std::string;

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
    
    // Text only
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

struct TextureSet
{
    std::shared_ptr<std::vector<Texture>> textures {};
    int tileNumWidth {0};
    int tileNumHeight {0};

    std::map<TileID, glm::vec2> tileCoords;

    template <typename A, typename B>
    glm::vec2 getTileCoords(A a, B b)
    {
        std::ostringstream query;
		query << a << "_" << b;
		return tileCoords[query.str()];
    }
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
