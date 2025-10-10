#include "texture-loader.h"

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "nlohmann/json.hpp"

//#include "Logger.h"
//#define LOG(severity, msg) Logger::log("Texture_Loader.cpp", severity, msg)

#define TEXTURE_DIR "/home/jbraun/projects/blockregion/textures/"

Texture_Loader::Texture_Loader()
{
    //LOG(INFO, "Starting to load textures");

    loadTexturesFromJson();

    // Error texture
    // textureLoader->loadTexture("error", "error.png");

    // Block Textures
    // textureLoader->loadTexture("blocks_diffuse", "blocks_diffuse.png");
    // textureLoader->loadTexture("blocks_specular", "blocks_specular.png");

    //LOG(INFO, "Finished loading textures");
}

Texture_Loader::~Texture_Loader()
{
    //LOG(INFO, "Deleting");
}

void Texture_Loader::loadTexturesFromJson()
{
    using json = nlohmann::json;

    // Construct path to textures.json
    std::ostringstream jsonPath;
    jsonPath << TEXTURE_DIR << "textures.json";

    // Check if file exists
    struct stat buffer;
    if (stat(jsonPath.str().c_str(), &buffer) != 0) {
        std::cerr << "Cannot find textures JSON, no textures will be loaded. Path: " << jsonPath.str() << std::endl;
        return;
    }

    // Open and parse the JSON file
    std::ifstream textureJson(jsonPath.str());
    if (!textureJson.is_open()) {
        std::cerr << "Failed to open textures JSON file: " << jsonPath.str() << std::endl;
        return;
    }

    //try {
        json textures = json::parse(textureJson);

        // Example: print the JSON
        std::cout << "Loaded textures: " << textures.dump(4) << std::endl;
        
        // You can now use `textures` as needed...
    //} catch (const json::parse_error& e) {
    //    std::cerr << "JSON parsing error: " << e.what() << std::endl;
    //    return;
    //}
    //LOG(DEBUG, textures.dump(4));
    json textureArray = *(textures.find("textures"));

    for (json::iterator tx = textureArray.begin(); tx != textureArray.end(); ++tx)
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

        if (hasSpecular)
        {
            loadTexture(name + "_diffuse", name, tileWidth, tileHeight);
            loadTexture(name + "_specular", name, tileWidth, tileHeight);
        }
        else
        {
            loadTexture(name, name, tileWidth, tileHeight);
        }
    }
}

void Texture_Loader::loadTexture(std::string name, std::string shaderName, int tileWidth, int tileHeight)
{
    // Find path to texture
    const std::string path = TEXTURE_DIR + name + ".png";

    GLuint textureMap;
    glGenTextures(1, &textureMap);

    int textureWidth, textureHeight;
    unsigned char *image;

    // Load in image from disk
    image = SOIL_load_image(path.c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
    if (image == NULL)
    {
        //LOG(WARN, "Failed to load texture from disk: \"" + name + "\" from path: " + path);
        return;
    }

    // Bind image data to OpenGL for use on graphics card
    glBindTexture(GL_TEXTURE_2D, textureMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Load texture id for later use
    this->loadedTextures.insert(make_pair(name, textureMap));

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
    this->textureTileDimensions.emplace(shaderName, std::make_pair(tileNumWidth, tileNumHeight));

    //LOG(INFO, "Loaded texture: \"" + name + "\" from path: " + path);
}

GLuint Texture_Loader::getTextureId(std::string textureName)
{
    if (this->loadedTextures.count(textureName) > 0)
    {
        return this->loadedTextures[textureName];
    }
    else
    {
        //const std::string log = "Unknown texture name: " + textureName;
        //LOG(WARN, log);
        return this->loadedTextures["error"];
    }
}

std::vector<Texture> Texture_Loader::getDiffAndSpecTextures(std::string prefix)
{
    std::vector<Texture> textures;
    Texture texture;

    texture.id = getTextureId(prefix + "_diffuse");
    texture.type = "texture_diffuse";
    textures.push_back(texture);

    texture.id = getTextureId(prefix + "_specular");
    texture.type = "texture_specular";
    textures.push_back(texture);

    return textures;
}

std::pair<int, int> Texture_Loader::getTextureTileDimensions(std::string name) {
    if (this->textureTileDimensions.count(name) > 0) {
        return this->textureTileDimensions[name];
    }
    else {
        //LOG(WARN, "Unable to find texture tile dimensions for \"" + name + "\", using (1, 1)");
        return std::make_pair(1, 1);
    }
}