#include "chunk.h"

Chunk::Chunk(Block_Consts* blockConsts, int xPos, int zPos) {
	blocks = std::vector<std::vector<std::vector<Block>>>(CHUNK_MAX_WIDTH, std::vector<std::vector<Block>>(CHUNK_MAX_HEIGHT, std::vector<Block>(CHUNK_MAX_WIDTH, Block())));
	this->blockConsts = blockConsts;
	this->xPos = xPos;
	this->zPos = zPos;
	this->toDelete = false;
}

Chunk::~Chunk() {
}

void Chunk::addBlock(glm::vec3 relPos, Block_Type type) {
	blocks[(int)relPos.x][(int)relPos.y][(int)relPos.z].setType(type);
}

void Chunk::addBlock(int x, int y, int z, Block_Type type) {
	blocks[x][y][z].setType(type);
}

Block* Chunk::getBlock(int x, int y, int z) {
	if (x < 0 || x >= CHUNK_MAX_WIDTH || y < 0 || y >= CHUNK_MAX_HEIGHT || z < 0 || z >= CHUNK_MAX_WIDTH) {
		//std::ostringstream msg;
		//msg << "getBlock error x=" << x << " y=" << y << " z=" << z;
		//LOG(WARN, msg.str());
		return nullptr;
	}

	return &blocks[x][y][z];
}

int Chunk::getXPos() {
	return xPos * CHUNK_MAX_WIDTH;
}

int Chunk::getZPos() {
	return zPos * CHUNK_MAX_WIDTH;
}

int Chunk::getChunkXPos() {
	return xPos;
}

int Chunk::getChunkZPos() {
	return zPos;
}

bool Chunk::shouldDelete() {
	return toDelete;
}

void Chunk::setDelete() {
	toDelete = true;
}