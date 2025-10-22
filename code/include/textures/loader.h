#pragma once

#include <string>
#include <ostream>
#include <vector>
#include <map>
#include <optional>
#include <iostream>
#include <sys/stat.h>

#include <errors/br-expected.h>

#include <textures/texture.h>

#include <textures/error-texture.h>

#include "nlohmann/json.hpp"
#include "SOIL2/SOIL2.h"

namespace textures
{

class Loader
{
public:
    Loader();

    TextureSet getTextureSet(const std::string name);

private:
    errors::expected<> parseJson(nlohmann::json textureArray);
    errors::expected<> loadTexture(std::string name, TextureType type,
        int tileWidth, int tileHeight);
    errors::expected<GLuint> bindTexture(unsigned char* image,
        int textureWidth, int textureHeight);
    errors::expected<> addTexture(std::string name, Texture& texture, int tileWidth, int tileHeight);

    errors::expected<int> getTileNumWithinTexture(int textureSize, int tileSize);

    void loadErrorTextures();

    std::map<std::string, TextureSet> textureSets;
};

/********************************
 * Definitions
********************************/

inline
Loader::Loader()
{
    loadErrorTextures();

    // Construct path to textures.json
    std::ostringstream jsonPath;
    jsonPath << TEXTURES_PATH << "textures.json";

    // Check if file exists
    struct stat buffer;
    if (stat(jsonPath.str().c_str(), &buffer) != 0)
    {
        std::cerr << "Cannot find textures JSON, no textures will be loaded. Path: " << jsonPath.str() << std::endl;
        return;
    }

    // Open and parse the JSON file
    std::ifstream textureJson(jsonPath.str());
    if (!textureJson.is_open())
    {
        std::cerr << "Failed to open textures JSON file: " << jsonPath.str() << std::endl;
        return;
    }

    try
    {
        nlohmann::json textures = nlohmann::json::parse(textureJson);
        
        // You can now use `textures` as needed...
        errors::expected<> retParse = parseJson(*(textures.find("textures")));
        if (errors::has_error(retParse))
        {
            std::cout << "Failed to load textures from JSON: " << retParse.error() << std::endl;
            return;
        }
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }
}

inline
TextureSet
Loader::getTextureSet(const std::string name)
{
    if (auto tsIter = textureSets.find(name); tsIter != textureSets.end())
    {
        return tsIter->second;
    }
    
    // Not found!
    // Return error texture loaded from memory
    return textureSets["error"];
}

inline
errors::expected<>
Loader::parseJson(nlohmann::json textureArray)
{
    const std::string keyName {"name"};
    const std::string keyType {"type"};
    const std::string keyTileWidth {"tileWidth"};
    const std::string keyTileHeight {"tileHeight"};

    for (nlohmann::json::iterator tx = textureArray.begin(); tx != textureArray.end(); ++tx)
    {
        // Name field check
        if (!tx->contains(keyName))
        {
            return errors::unexpected("Missing name field for texture in JSON:\n\r" + tx->dump(4), errors::Code::InvalidArgument);
        }
        std::string name = *(tx->find(keyName));

        // Has specular field check
        if (!tx->contains(keyType))
        {
            return errors::unexpected("Missing type field for texture in JSON:\n\r" + tx->dump(4), errors::Code::InvalidArgument);
        }
        std::string typeStr = *(tx->find(keyType));
        TextureType type = toTextureType(typeStr);

        // Slice width field check
        int tileWidth = -1;
        if (tx->contains(keyTileWidth))
        {
            tileWidth = *(tx->find(keyTileWidth));
        }
        else
        {
            return errors::unexpected("Missing tile width field for texture in JSON:\n\r" + tx->dump(4), errors::Code::InvalidArgument);
        }

        // Slice height field check
        int tileHeight = -1;
        if (tx->contains(keyTileHeight))
        {
            tileHeight = *(tx->find(keyTileHeight));
        }
        else
        {
            return errors::unexpected("Missing tile height field for texture in JSON:\n\r" + tx->dump(4), errors::Code::InvalidArgument);
        }

        errors::expected<> retLoad = loadTexture(name, type, tileWidth, tileHeight);
        if (errors::has_error(retLoad))
        {
            return errors::unexpected(retLoad.error());
        }
    }

    return {};
}

inline
errors::expected<>
Loader::loadTexture(std::string name, 
    TextureType type, int tileWidth, int tileHeight)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load start: " << err << std::endl;
    }

    // Find path to texture
    std::ostringstream path;
    path << TEXTURES_PATH << name;
    switch(type)
    {
        case TextureType::Diffuse:
            path << "_diffuse";
            break;
        case TextureType::Specular:
            path << "_specular";
            break;
        default:
            break;
    }
    path << ".png";

    int textureWidth, textureHeight;
    unsigned char *image;

    // Load in image from disk
    image = SOIL_load_image(path.str().c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
    if (image == NULL)
    {
        //std::cout << "Failed to load texture from disk: " << path.str() << std::endl;
        return errors::unexpected("Failed to load texture from disk: " + path.str(), errors::Code::InvalidArgument);
    }

    errors::expected<GLuint> retBind = bindTexture(image, textureWidth, textureHeight);
    if (errors::has_error(retBind))
    {
        return errors::unexpected(retBind.error());
    }

    Texture tex;
    tex.type = type;
    // Load texture id for later use
    tex.id = retBind.value();

    // Calculate tile dimensions
    errors::expected<int> tileNumWidth = getTileNumWithinTexture(textureWidth, tileWidth);
    if (errors::has_error(tileNumWidth))
    {
        return errors::unexpected(tileNumWidth.error().prefix("Tile width deduction failed for ["+path.str()+"]: "));
    }

    errors::expected<int> tileNumHeight = getTileNumWithinTexture(textureHeight, tileHeight);
    if (errors::has_error(tileNumHeight))
    {
        return errors::unexpected(tileNumHeight.error().prefix("Tile height deduction failed for ["+path.str()+"]: "));
    }

    return addTexture(name, tex, tileNumWidth.value(), tileNumHeight.value());
}

inline
errors::expected<GLuint>
Loader::bindTexture(unsigned char* image,
    int textureWidth, int textureHeight)
{
    GLuint textureMap;
    glGenTextures(1, &textureMap);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to generate texture object with error: " + std::to_string(err), errors::Code::InitializationError);
    }

    // Bind image data to OpenGL for use on graphics card
    glBindTexture(GL_TEXTURE_2D, textureMap);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to bind texture with error: " + std::to_string(err), errors::Code::InitializationError);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to load texture image with error: " + std::to_string(err), errors::Code::InitializationError);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to generate mipmap with error: " + std::to_string(err), errors::Code::InitializationError);
    }
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to set wrap S parameter with error: " + std::to_string(err), errors::Code::InitializationError);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to set wrap T parameter with error: " + std::to_string(err), errors::Code::InitializationError);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to set min filter parameter with error: " + std::to_string(err), errors::Code::InitializationError);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return errors::unexpected("OpenGL: Failed to set mag filter parameter with error: " + std::to_string(err), errors::Code::InitializationError);
    }

    return textureMap;
}

inline
errors::expected<>
Loader::addTexture(std::string name, Texture& texture, int tileWidth, int tileHeight)
{
    if (auto tsIter = textureSets.find(name); tsIter != textureSets.end())
    {
        // Error checking
        if (tsIter->second.tileWidth != tileWidth)
        {
            std::string errMsg {"Textures have different tile widths for ["};
            errMsg += name;
            errMsg += "]: Existing=";
            errMsg += std::to_string(tsIter->second.tileWidth);
            errMsg += " New=";
            errMsg += std::to_string(tileWidth);
            return errors::unexpected(errMsg, errors::Code::InvalidArgument);
        }
        if (tsIter->second.tileHeight != tileHeight)
        {
            std::string errMsg {"Textures have different tile heights for ["};
            errMsg += name;
            errMsg += "]: Existing=";
            errMsg += std::to_string(tsIter->second.tileHeight);
            errMsg += " New=";
            errMsg += std::to_string(tileHeight);
            return errors::unexpected(errMsg, errors::Code::InvalidArgument);
        }

        tsIter->second.textures->emplace_back(texture);
    }
    else
    {
        // Create new entry
        TextureSet texSet;
        texSet.tileWidth = tileWidth;
        texSet.tileHeight = tileHeight;
        texSet.textures = std::make_shared<std::vector<Texture>>();
        texSet.textures->emplace_back(texture);
        textureSets[name] = texSet;
    }

    return {};
}

inline
errors::expected<int>
Loader::getTileNumWithinTexture(int textureSize, int tileSize)
{
    if (tileSize == -1) {
        // This means that only 1 texture is in this tile
        return 1;
    }
    else if (tileSize <= 0)
    {
        // Invalid arguments
        return errors::unexpected("Invalid Tile Size=" + std::to_string(tileSize), errors::Code::InvalidArgument);
    }
    else if (textureSize % tileSize != 0)
    {
        // The textures do not evenly fit within the tile
        // This tile is malformed
        std::string errMsg {"Textures do you evenly tile: Texture Size="};
        errMsg += std::to_string(textureSize);
        errMsg += " Tile Size=";
        errMsg += std::to_string(tileSize);
        return errors::unexpected(errMsg, errors::Code::InvalidArgument);
    }
    // Error checks are done
    
    return textureSize / tileSize;
}

inline
void
Loader::loadErrorTextures()
{
    int textureWidth, textureHeight;
    unsigned char *image;

    image = SOIL_load_image_from_memory(images::_textures_error_png, images::_textures_error_png_len,
        &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
    if (image == NULL)
    {
        //LOG(WARN, "Failed to load texture from disk: \"" + name + "\" from path: " + path);
        std::cout << "Failed to load texture from memory: " << "error" << std::endl;
        return;
    }

    errors::expected<int> texId = bindTexture(image, textureWidth, textureHeight);
    if (errors::has_error(texId))
    {
        std::cout << "Failed to bind error texture: " << texId.error() << std::endl;
        return;
    }

    Texture tex;
    tex.type = TextureType::Error;
    // Load texture id for later use
    tex.id = texId.value();

    errors::expected<> retAdd = addTexture("error", tex, 1, 1);
    if (errors::has_error(retAdd))
    {
        std::cout << "Failed to add error texture to map: " << retAdd.error() << std::endl;
        return;
    }
}

} // namespace textures
