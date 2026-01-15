#pragma once

#include <string>
#include <ostream>
#include <fstream>
#include <vector>
#include <map>
#include <optional>
#include <iostream>
#include <sys/stat.h>
#include <filesystem>

#include <errors/br-expected.h>
#include <textures/texture.h>
#include <utils/get-folder.h>

#include <textures/error-texture.h>

#include "nlohmann/json.hpp"
#include "SOIL2/SOIL2.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <spdlog/spdlog.h>

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
        int tileWidth, int tileHeight, std::map<TileID, glm::vec2> tileCoords);
    errors::expected<GLuint> bindTexture(unsigned char* image,
        int textureWidth, int textureHeight);
    errors::expected<> addTexture(std::string name, Texture& texture,
        int tileWidth, int tileHeight, std::map<TileID, glm::vec2> tileCoords);

    errors::expected<> loadText(std::string name);

    errors::expected<int> getTileNumWithinTexture(int textureSize, int tileSize);

    void loadErrorTextures();

    std::map<std::string, TextureSet> textureSets;

    std::shared_ptr<spdlog::logger> m_logger;
};

/********************************
 * Definitions
********************************/

inline
Loader::Loader():
    m_logger(spdlog::get("blockregion"))
{
    loadErrorTextures();

    // Construct path to textures.json
    std::filesystem::path texturePath = utils::get_executable_dir().append(TEXTURES_PATH).append("textures.json");

    // Check if file exists
    struct stat buffer;
    if (stat(texturePath.string().c_str(), &buffer) != 0)
    {
        m_logger->error("Cannot find textures JSON, no textures will be loaded. Path: {}", texturePath.string());
        return;
    }

    // Open and parse the JSON file
    std::ifstream textureJson(texturePath.string());
    if (!textureJson.is_open())
    {
        m_logger->error("Failed to open textures JSON file: {}", texturePath.string());
        return;
    }

    try
    {
        nlohmann::json textures = nlohmann::json::parse(textureJson);
        
        // You can now use `textures` as needed...
        errors::expected<> retParse = parseJson(*(textures.find("textures")));
        if (errors::has_error(retParse))
        {
            m_logger->error("Failed to load textures from JSON: {}", retParse.error());
            return;
        }
    }
    catch (const nlohmann::json::parse_error& e)
    {
        m_logger->error("JSON parsing error: {}", e.what());
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

    m_logger->warn("Could not find texture [{}]", name);
    
    // Not found!
    // Return error texture loaded from memory
    return textureSets["error"];
}

inline
errors::expected<>
Loader::parseJson(nlohmann::json textureArray)
{
    const std::string keyName {"name"};
    const std::string keyTypes {"types"};
    const std::string keyTileWidth {"tileWidth"};
    const std::string keyTileHeight {"tileHeight"};
    const std::string keyTileCoords {"tileCoords"};
    const std::string keyX {"x"};
    const std::string keyY {"y"};
    const std::string keySides {"sides"};

    for (nlohmann::json::iterator tx = textureArray.begin(); tx != textureArray.end(); ++tx)
    {
        // Name field check
        if (!tx->contains(keyName))
        {
            return errors::unexpected("Missing name field for texture in JSON:\n\r" + tx->dump(4), errors::Code::InvalidArgument);
        }
        std::string name = *(tx->find(keyName));

        // Has specular field check
        if (!tx->contains(keyTypes))
        {
            return errors::unexpected("Missing types field for texture in JSON:\n\r" + tx->dump(4), errors::Code::InvalidArgument);
        }
        std::vector<std::string> typeStrs = *(tx->find(keyTypes));

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

        std::map<TileID, glm::vec2> tileCoords;
        if (tx->contains(keyTileCoords))
        {
            for (auto tile: *(tx->find(keyTileCoords)))
            {
                glm::vec2 tileLoc = glm::vec2(*(tile.find(keyX)), *(tile.find(keyY)));
                for (auto side: *(tile.find(keySides)))
                {
                    tileCoords[side] = tileLoc;
                }
            }
        }

        for (auto typeStr: typeStrs)
        {
            TextureType type = toTextureType(typeStr);
            if (type == TextureType::Text)
            {
                errors::expected<> retLoad = loadText(name);
                if (errors::has_error(retLoad))
                {
                    return errors::unexpected(retLoad.error());
                }
            }
            else
            {
                errors::expected<> retLoad = loadTexture(name, type,
                    tileWidth, tileHeight, tileCoords);
                if (errors::has_error(retLoad))
                {
                    return errors::unexpected(retLoad.error());
                }
            }
        }
    }

    return {};
}

inline
errors::expected<>
Loader::loadTexture(std::string name, 
    TextureType type, int tileWidth, int tileHeight,
    std::map<TileID, glm::vec2> tileCoords)
{
    // Find path to texture
    std::filesystem::path texturePath = utils::get_executable_dir().append(TEXTURES_PATH).append(name);
    switch(type)
    {
        case TextureType::Diffuse:
            texturePath += "_diffuse";
            break;
        case TextureType::Specular:
            texturePath += "_specular";
            break;
        default:
            break;
    }
    texturePath += ".png";

    // Check if file exists
    struct stat buffer;
    if (stat(texturePath.string().c_str(), &buffer) != 0)
    {
        return errors::unexpected(fmt::format("Texture [{}] file not found from disk: [{}]", name, texturePath.string()), errors::Code::NotFound);
    }

    int textureWidth, textureHeight;
    unsigned char *image;

    // Load in image from disk
    image = SOIL_load_image(texturePath.string().c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
    if (image == NULL)
    {
        return errors::unexpected("Failed to load texture from disk: " + texturePath.string(), errors::Code::InvalidArgument);
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
        return errors::unexpected(tileNumWidth.error().prefix("Tile width deduction failed for ["+name+"]: "));
    }

    errors::expected<int> tileNumHeight = getTileNumWithinTexture(textureHeight, tileHeight);
    if (errors::has_error(tileNumHeight))
    {
        return errors::unexpected(tileNumHeight.error().prefix("Tile height deduction failed for ["+name+"]: "));
    }

    // Normalize each tileCoords vector to be a fraction with the
    // total number of tiles as the denominator
    for (auto tile: tileCoords)
    {
        tileCoords[tile.first].x = tile.second.x / tileNumWidth.value();
        tileCoords[tile.first].y = tile.second.y / tileNumHeight.value();
    }

    return addTexture(name, tex, tileNumWidth.value(), tileNumHeight.value(), std::move(tileCoords));
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
Loader::addTexture(std::string name, Texture& texture,
    int tileNumWidth, int tileNumHeight, std::map<TileID, glm::vec2> tileCoords)
{
    if (auto tsIter = textureSets.find(name); tsIter != textureSets.end())
    {
        // Error checking for tile count
        if (tsIter->second.tileNumWidth != tileNumWidth)
        {
            std::string errMsg {"Textures have different tile widths for ["};
            errMsg += name;
            errMsg += "]: Existing=";
            errMsg += std::to_string(tsIter->second.tileNumWidth);
            errMsg += " New=";
            errMsg += std::to_string(tileNumWidth);
            return errors::unexpected(errMsg, errors::Code::InvalidArgument);
        }
        if (tsIter->second.tileNumHeight != tileNumHeight)
        {
            std::string errMsg {"Textures have different tile heights for ["};
            errMsg += name;
            errMsg += "]: Existing=";
            errMsg += std::to_string(tsIter->second.tileNumHeight);
            errMsg += " New=";
            errMsg += std::to_string(tileNumHeight);
            return errors::unexpected(errMsg, errors::Code::InvalidArgument);
        }
        
        // Error checking for tile coordinates
        if (tsIter->second.tileCoords.size() != tileCoords.size())
        {
            std::string errMsg {"Tile Coordinates maps have different sizes for ["};
            errMsg += name;
            errMsg += "]: Existing=";
            errMsg += std::to_string(tsIter->second.tileCoords.size());
            errMsg += " New=";
            errMsg += std::to_string(tileCoords.size());
            return errors::unexpected(errMsg, errors::Code::InvalidArgument);
        }
        if (tsIter->second.tileCoords != tileCoords)
        {
            return errors::unexpected("Tile Coordinates maps don't match for ["+name+"]", errors::Code::InvalidArgument);
        }

        // Exisiting tile info does not conflict with this tile
        tsIter->second.textures->emplace_back(texture);

        m_logger->debug("Added texture [{}] to existing entry", name);
    }
    else
    {
        // Create new entry
        TextureSet texSet;
        texSet.tileNumWidth = tileNumWidth;
        texSet.tileNumHeight = tileNumHeight;
        texSet.textures = std::make_shared<std::vector<Texture>>();
        texSet.textures->emplace_back(texture);
        texSet.tileCoords = std::move(tileCoords);
        textureSets[name] = texSet;

        m_logger->debug("Added texture [{}] as a new entry", name);
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
        m_logger->error("Failed to load texture from memory: {}", "error");
        return;
    }

    errors::expected<int> texId = bindTexture(image, textureWidth, textureHeight);
    if (errors::has_error(texId))
    {
        m_logger->error("Failed to bind error texture: {}", texId.error());
        return;
    }

    Texture tex;
    tex.type = TextureType::Error;
    // Load texture id for later use
    tex.id = texId.value();

    std::map<TileID, glm::vec2> tileCoords;

    errors::expected<> retAdd = addTexture("error", tex, 1, 1, std::move(tileCoords));
    if (errors::has_error(retAdd))
    {
        m_logger->error("Failed to add error texture to map: {}", retAdd.error());
        return;
    }
}

inline
errors::expected<>
Loader::loadText(std::string name)
{
    int fontSize = 48;

    FT_Library ft;
    auto errorFtInit = FT_Init_FreeType(&ft);
    if (errorFtInit)
    {
        return errors::unexpected(fmt::format("Failed to initialize Freetype library: 0x{:x}", errorFtInit), errors::Code::InitializationError);
    }

    std::filesystem::path textPath = utils::get_executable_dir().append(TEXTURES_PATH).append(name + ".ttf");

    FT_Face face;
    auto errorFaceNew = FT_New_Face(ft, textPath.string().c_str(), 0, &face);
    if (errorFaceNew)
    {
        return errors::unexpected(fmt::format("Failed to load text [{}] from disk with error 0x{:x}: [{}]", name, errorFaceNew, textPath.string()), errors::Code::InvalidArgument);
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, fontSize);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        TextureSet texSet;
        texSet.textures = std::make_shared<std::vector<Texture>>();

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                m_logger->error("Failed to load Glyph [0x{:x}]", c);
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Texture tex;
            tex.id = texture;
            tex.type = TextureType::Text;
            tex.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            tex.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            tex.advance = static_cast<unsigned int>(face->glyph->advance.x);

            texSet.textures->emplace_back(tex);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        textureSets[name] = texSet;
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return {};
}

} // namespace textures
