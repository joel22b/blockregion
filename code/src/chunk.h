#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <mutex>

#include <sys/stat.h>

#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include "block-consts.h"
#include "chunk-consts.h"
#include "chunk-mesh.h"
#include "block.h"

#include "shaders/block.h"

class Chunk {
private:
	std::vector<std::vector<std::vector<Block>>> blocks;
	int xPos, zPos;
	bool toDelete;

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
