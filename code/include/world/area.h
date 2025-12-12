#pragma once

#include <vector>
#include <functional>

#include <world/chunk.h>

#include <renderer/wrapper.h>

namespace world
{

constexpr uint16_t AREA_DEFAULT_RADIUS {3};

//typedef renderer::Wrapper<Chunk> (*GetChunkCallback)(int x, int y);
using GetChunkCallback = std::function<std::shared_ptr<renderer::Wrapper<world::Chunk>>(int, int)>;

class Area
{
public:
    Area(GetChunkCallback getChunk, int offsetX = 0, int offsetZ = 0, uint16_t radius = AREA_DEFAULT_RADIUS);
    ~Area();

    errors::expected<> changeRadius(uint16_t newRadius);

private:
    uint16_t getAreaSize();

    int offsetX {0};
    int offsetZ {0};

    uint16_t radius {AREA_DEFAULT_RADIUS};

    std::vector<std::vector<std::shared_ptr<renderer::Wrapper<Chunk>>>> chunkMatrix;

    GetChunkCallback getChunk;
};

/********************************
 * Definitions
********************************/

inline
Area::Area(GetChunkCallback getChunk, int offsetX, int offsetZ, uint16_t radius) :
    getChunk(getChunk), offsetX(offsetX), offsetZ(offsetZ), radius(radius)
{
    // Reserve the space needed
    uint16_t areaSize = getAreaSize();

    // Load in the chunks
    for (uint16_t i = 0; i < areaSize; i++)
    {
        chunkMatrix.emplace_back(std::vector<std::shared_ptr<renderer::Wrapper<Chunk>>>());
        
        for (uint16_t j = 0; j < areaSize; j++)
        {
            std::shared_ptr<renderer::Wrapper<world::Chunk>> newChunk = getChunk(offsetX + (i - radius), offsetZ + (j - radius));
            chunkMatrix[i].emplace_back(newChunk);
        }
    }
}

inline
Area::~Area()
{}

inline
errors::expected<>
Area::changeRadius(uint16_t newRadius)
{
    return {};
}

inline
uint16_t
Area::getAreaSize()
{
    return (radius*2) + 1;
}

} // namespace world
