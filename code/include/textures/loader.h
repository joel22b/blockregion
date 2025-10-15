#pragma once

#include <string>
#include <ostream>
#include <vector>
#include <map>
#include <optional>
#include <iostream>
#include <sys/stat.h>

#include <textures/texture.h>

#include "nlohmann/json.hpp"
#include "SOIL2/SOIL2.h"

namespace textures
{

class Loader
{
public:
    Loader();

    std::vector<Texture> getTextures(const std::string name);
    Tile getTile(const std::string name);

private:
    void parseJson(nlohmann::json textureArray);
    void loadTexture(std::string name, TextureType type,
        int tileWidth, int tileHeight);

    std::map<std::string, std::vector<Texture>> texturesMap;
    std::map<std::string, Tile> tilesMap;
};

/********************************
 * Definitions
********************************/

inline
Loader::Loader()
{
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

        // Example: print the JSON
        std::cout << "Loaded textures: " << textures.dump(4) << std::endl;
        
        // You can now use `textures` as needed...
        parseJson(*(textures.find("textures")));
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }
}

inline
std::vector<Texture>
Loader::getTextures(const std::string name)
{
    std::cout << "getTextures: " << name << std::endl;
    if (auto texIter = texturesMap.find(name); texIter != texturesMap.end())
    {
        return texIter->second;
    }

    // Not found!
    std::cout << "Texture array not found in loader: " << name << std::endl;
    return std::vector<Texture>();
}

inline
Tile
Loader::getTile(const std::string name)
{
    std::cout << "getTile: " << name << std::endl;
    if (auto tileIter = tilesMap.find(name); tileIter != tilesMap.end())
    {
        return tileIter->second;
    }
    
    // Not found!
    std::cout << "Tile not found in texture loader: " << name << std::endl;
    return Tile();
}

inline
void
Loader::parseJson(nlohmann::json textureArray)
{
    for (nlohmann::json::iterator tx = textureArray.begin(); tx != textureArray.end(); ++tx)
    {
        // Name field check
        if (!tx->contains("name"))
        {
            //LOG(WARN, "Missing name field for texture in JSON, skipping...\n\r" + tx->dump(4));
            continue;
        }
        std::string name = *(tx->find("name"));

        // Has specular field check
        bool hasSpecular = false;
        if (tx->contains("hasSpecular"))
        {
            hasSpecular = *(tx->find("hasSpecular"));
        }
        else
        {
            //LOG(WARN, "Missing hasSpecular field for texture in JSON, assuming false. Name: " + name);
        }

        // Slice width field check
        int tileWidth = -1;
        if (tx->contains("tileWidth"))
        {
            tileWidth = *(tx->find("tileWidth"));
        }
        else
        {
            //LOG(WARN, "Missing tileWidth field for texture in JSON, assuming -1. Name: " + name);
        }

        // Slice height field check
        int tileHeight = -1;
        if (tx->contains("tileHeight"))
        {
            tileHeight = *(tx->find("tileHeight"));
        }
        else
        {
            //LOG(WARN, "Missing tileHeight field for texture in JSON, assuming -1. Name: " + name);
        }

        loadTexture(name, TextureType::Defuse, tileWidth, tileHeight);

        if (hasSpecular)
        {
            
            loadTexture(name, TextureType::Specular, tileWidth, tileHeight);
        }
    }
}

inline
void
Loader::loadTexture(std::string name, 
    TextureType type, int tileWidth, int tileHeight)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load start: " << err << std::endl;
    }

    Texture tex;
    tex.type = type;

    // Find path to texture
    std::ostringstream path;
    path << TEXTURES_PATH << name;
    switch(type)
    {
        case TextureType::Defuse:
            path << "_diffuse";
            break;
        case TextureType::Specular:
            path << "_specular";
            break;
        default:
            break;
    }
    path << ".png";

    GLuint textureMap;
    glGenTextures(1, &textureMap);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load create: " << err << std::endl;
    }

    int textureWidth, textureHeight;
    unsigned char *image;

    // Load in image from disk
    image = SOIL_load_image(path.str().c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
    if (image == NULL)
    {
        //LOG(WARN, "Failed to load texture from disk: \"" + name + "\" from path: " + path);
        std::cout << "Failed to load texture: " << path.str() << std::endl;
        return;
    }

    // Bind image data to OpenGL for use on graphics card
    glBindTexture(GL_TEXTURE_2D, textureMap);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load bind: " << err << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load 2D: " << err << std::endl;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load mipmap: " << err << std::endl;
    }
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load s wrap: " << err << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load t wrap: " << err << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load min: " << err << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error tex load mag: " << err << std::endl;
    }

    // Load texture id for later use
    tex.id = textureMap;
    std::cout << "Texture ID: " << textureMap << std::endl;

    // Calculate tile dimensions
    int tileNumWidth = 1;
    if (tileWidth > 0) {
        // Using integer division to round down
        tileNumWidth = textureWidth / tileWidth;

        if (textureWidth % tileWidth != 0) {
            //std::ostringstream log;
            //log << "textureWidth=" << textureWidth << " tileWidth=" << tileWidth << " are not evenly divisible, rounding down. Name=" << shaderName;
            //LOG(WARN, log.str());
        }
    }

    int tileNumHeight = 1;
    if (tileHeight > 0) {
        // Using integer division to round down
        tileNumHeight = textureHeight / tileHeight;

        if (textureHeight % tileHeight != 0) {
            //std::ostringstream log;
            //log << "textureWidth=" << textureHeight << " tileWidth=" << tileHeight << " are not evenly divisible, rounding down. Name=" << shaderName;
            //LOG(WARN, log.str());
        }
    }
    //std::ostringstream log;
    //log << "Setting tile dimensions for name=" << shaderName << " width=" << tileNumWidth << " height=" << tileNumHeight;
    //LOG(DEBUG, log.str());
    Tile tile;
    tile.totalWidth = tileWidth;
    tile.totalHeight = tileHeight;
    tile.numWidth = tileNumWidth;
    tile.numHeight = tileNumHeight;
    // TODO: add checks
    tilesMap[name] = tile;

    if (auto texIter = texturesMap.find(name); texIter != texturesMap.end())
    {
        texIter->second.emplace_back(tex);
    }
    else
    {
        std::vector<Texture> texs;
        texs.emplace_back(tex);
        texturesMap[name] = texs;
    }
}

} // namespace textures
