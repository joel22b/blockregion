#pragma once

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <functional>

#include <GL/glew.h>

#include "SOIL2/SOIL2.h"

struct Texture {
    GLuint id;
    std::string type;
};

class Texture_Loader {
private:
    std::map<std::string, GLuint> loadedTextures;
    std::map<std::string, std::pair<int, int>> textureTileDimensions;

    void loadTexturesFromJson();
    void loadTexture(std::string name, std::string shaderName, int tileWidth, int tileHeight);

public:
	Texture_Loader();
    ~Texture_Loader();

    GLuint getTextureId(std::string textureName);

    std::vector<Texture> getDiffAndSpecTextures(std::string prefix);
    std::pair<int, int> getTextureTileDimensions(std::string name);
};