#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <mutex>

#include <sys/stat.h>

#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include "world/block.h"
#include "world/types.h"

#include <renderer/types.h>

namespace world
{

class Chunk {
private:
	std::vector<std::vector<std::vector<Block>>> blocks;
	ChunkCoord coord;
	bool toDelete {false};

	renderer::RenderId renderId;

public:
	Chunk(ChunkCoord coord);
	~Chunk();

	ChunkCoord getCoord() const;

	void addBlock(ChunkInternalCoord blockCoord, Block_Type type);
	Block* getBlock(ChunkInternalCoord blockCoord);

	bool shouldDelete();
	void setDelete();
};

/********************************
 * Definitions
********************************/

inline
Chunk::Chunk(ChunkCoord coord) :
	coord(coord)
{
	blocks = std::vector<std::vector<std::vector<Block>>>
		(consts::CHUNK_MAX_WIDTH, std::vector<std::vector<Block>>(consts::CHUNK_MAX_HEIGHT,
			std::vector<Block>(consts::CHUNK_MAX_WIDTH, Block())));
}

inline
Chunk::~Chunk()
{}

inline
ChunkCoord
Chunk::getCoord() const
{
	return coord;
}

inline
void
Chunk::addBlock(ChunkInternalCoord blockCoord, Block_Type type)
{
	blocks[blockCoord.x][blockCoord.y][blockCoord.z].setType(type);
}

inline
Block*
Chunk::getBlock(ChunkInternalCoord blockCoord)
{
	return &blocks[blockCoord.x][blockCoord.y][blockCoord.z];
}

inline
bool
Chunk::shouldDelete()
{
	return toDelete;
}

inline
void
Chunk::setDelete()
{
	toDelete = true;
}

} // namespace world
