#pragma once

#include <vector>
#include <math.h>
#include <string>
#include <thread>
#include <mutex>

#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include "chunk.h"
#include "chunk-consts.h"
#include "block-consts.h"
#include "texture-loader.h"
#include "shaders/block.h"

class World {
public:
	World();
	World(Texture_Loader* textureLoader);
	~World();

	void doUpdate();
	void doRender(shaders::Shader<shaders::Block> shader, GLint modelLoc);

	Chunk* getChunkByCoords(int xPos, int zPos);
	glm::vec2 getChunkCoords(int xPos, int zPos);
	Block* getBlock(int xPos, int yPos, int zPos);

	std::string getWorldFolder();

	void shiftChunks(int xPos, int zPos);
	void shiftChunksThread(Block_Consts* blockConsts, int xPos, int zPos);
	void updateChunkRenderDistance(int renderDistance, int bufferDistance, int xPos, int zPos);

private:
	Chunk** chunks;
	std::mutex chunksMutex;
	std::mutex shiftMutex;
	int chunkXOffset, chunkZOffset;
	int renderDistance, bufferDistance, chunksLength;
	Block_Consts* blockConsts;
	const std::string worldName = "test";

	void generateChunk(Chunk* chunk);
	void loadChunk(Chunk* chunk);
	void saveChunk(Chunk* chunk);
	void updateChunkNoOffset(int xPos, int zPos);
	void updateChunkNoOffset(Chunk* chunk);

	Chunk* getChunk(int xPos, int zPos);
	Chunk* getChunkNoOffset(int xPos, int zPos);

	inline bool fileExists(const std::string& name);
};