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

	void addBlock(glm::vec3 relPos, Block_Type type);
	void addBlock(int x, int y, int z, Block_Type type);
	Block* getBlock(int x, int y, int z);

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
Chunk::addBlock(glm::vec3 relPos, Block_Type type)
{
	blocks[(int)relPos.x][(int)relPos.y][(int)relPos.z].setType(type);
}

inline
void
Chunk::addBlock(int x, int y, int z, Block_Type type)
{
	blocks[x][y][z].setType(type);
}

inline
Block*
Chunk::getBlock(int x, int y, int z)
{
	if (x < 0 || x >= consts::CHUNK_MAX_WIDTH || y < 0 || y >= consts::CHUNK_MAX_HEIGHT || z < 0 || z >= consts::CHUNK_MAX_WIDTH)
	{
		//std::ostringstream msg;
		//msg << "getBlock error x=" << x << " y=" << y << " z=" << z;
		//LOG(WARN, msg.str());
		return nullptr;
	}

	return &blocks[x][y][z];
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
