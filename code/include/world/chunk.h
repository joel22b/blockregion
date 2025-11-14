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

#include "shaders/block.h"

namespace world
{

const int CHUNK_MAX_WIDTH = 16;
const int CHUNK_MAX_HEIGHT = 64;

class Chunk {
private:
	std::vector<std::vector<std::vector<Block>>> blocks;
	int xPos, zPos;
	bool toDelete {false};

public:
	Chunk(int xPos, int zPos);
	~Chunk();

	void addBlock(glm::vec3 relPos, Block_Type type);
	void addBlock(int x, int y, int z, Block_Type type);
	Block* getBlock(int x, int y, int z);

	int getXPos();
	int getZPos();

	int getChunkXPos();
	int getChunkZPos();

	bool shouldDelete();
	void setDelete();
};

/********************************
 * Definitions
********************************/

inline
Chunk::Chunk(int xPos, int zPos) :
	xPos(xPos), zPos(zPos)
{
	blocks = std::vector<std::vector<std::vector<Block>>>
		(CHUNK_MAX_WIDTH, std::vector<std::vector<Block>>(CHUNK_MAX_HEIGHT,
			std::vector<Block>(CHUNK_MAX_WIDTH, Block())));
}

inline
Chunk::~Chunk()
{}

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
	if (x < 0 || x >= CHUNK_MAX_WIDTH || y < 0 || y >= CHUNK_MAX_HEIGHT || z < 0 || z >= CHUNK_MAX_WIDTH)
	{
		//std::ostringstream msg;
		//msg << "getBlock error x=" << x << " y=" << y << " z=" << z;
		//LOG(WARN, msg.str());
		return nullptr;
	}

	return &blocks[x][y][z];
}

inline
int
Chunk::getXPos()
{
	return xPos * CHUNK_MAX_WIDTH;
}

inline
int
Chunk::getZPos()
{
	return zPos * CHUNK_MAX_WIDTH;
}

inline
int
Chunk::getChunkXPos()
{
	return xPos;
}

inline
int
Chunk::getChunkZPos()
{
	return zPos;
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
