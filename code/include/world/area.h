#pragma once

#include <vector>
#include <functional>

#include <world/chunk.h>

#include <renderer/wrapper.h>

namespace world
{

constexpr uint16_t AREA_DEFAULT_RADIUS {3};

//typedef renderer::Wrapper<Chunk> (*GetChunkCallback)(int x, int y);
using GetChunkCallback = std::function<std::shared_ptr<renderer::Wrapper<world::Chunk>>(ChunkCoord)>;

class Area
{
public:
    Area(GetChunkCallback getChunk, ChunkCoord coord, uint16_t radius = AREA_DEFAULT_RADIUS);
    ~Area();

    errors::expected<> changeRadius(uint16_t newRadius);

    bool operator==(const ChunkCoord& otherCoord);
    bool operator!=(const ChunkCoord& otherCoord);

private:
    uint16_t getAreaSize();

    ChunkCoord coord;

    uint16_t radius {AREA_DEFAULT_RADIUS};

    std::vector<std::vector<std::shared_ptr<renderer::Wrapper<Chunk>>>> chunkMatrix;

    GetChunkCallback getChunk;
};

/********************************
 * Definitions
********************************/

inline
Area::Area(GetChunkCallback getChunk, ChunkCoord coord, uint16_t radius) :
    getChunk(getChunk), coord(coord), radius(radius)
{
    // Reserve the space needed
    uint16_t areaSize = getAreaSize();

    // Load in the chunks
    for (uint16_t i = 0; i < areaSize; i++)
    {
        chunkMatrix.emplace_back(std::vector<std::shared_ptr<renderer::Wrapper<Chunk>>>());
        
        for (uint16_t j = 0; j < areaSize; j++)
        {
            ChunkCoord newCoord(coord.x + (i - radius), coord.z + (j - radius));
            std::shared_ptr<renderer::Wrapper<world::Chunk>> newChunk = getChunk(newCoord);
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
bool
Area::operator==(const ChunkCoord& otherCoord)
{
    return coord == otherCoord;
}

inline
bool
Area::operator!=(const ChunkCoord& otherCoord)
{
    return !(coord == otherCoord);
}

inline
uint16_t
Area::getAreaSize()
{
    return (radius*2) + 1;
}

} // namespace world
