#pragma once

#include <vector>
#include <math.h>
#include <string>
#include <thread>
#include <mutex>

#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include "world/chunk.h"
#include "world/block.h"
#include <world/area.h>

#include <renderer/renderer.h>

namespace world
{

struct ChunkData
{
	Chunk* chunk;
	renderer::RenderId renderId {0};

	~ChunkData()
	{
		delete chunk;
	}
};

class World {
public:
	World();
	World(std::shared_ptr<renderer::Renderer> _renderer);
	~World();

	//Chunk* getChunkByCoords(int xPos, int zPos);
	//glm::vec2 getChunkCoords(int xPos, int zPos);
	//Block* getBlock(int xPos, int yPos, int zPos);

	std::string getWorldFolder();

	errors::expected<> loadArea(int xPos, int zPos);

	//void shiftChunks(int xPos, int zPos);
	//void shiftChunksThread(int xPos, int zPos);
	//void updateChunkRenderDistance(int renderDistance, int bufferDistance, int xPos, int zPos);

private:
	ChunkData** chunks;
	std::mutex chunksMutex;
	std::mutex shiftMutex;
	int chunkXOffset, chunkZOffset;
	int renderDistance, bufferDistance, chunksLength;
	const std::string worldName = "test";

	std::shared_ptr<renderer::Wrapper<Chunk>> getChunk(int x, int y);

	void generateChunk(Chunk* chunk);
	void loadChunk(Chunk* chunk);
	void saveChunk(Chunk* chunk);
	//void updateChunkNoOffset(int xPos, int zPos);
	//void updateChunkNoOffset(Chunk* chunk);

	//Chunk* getChunk(int xPos, int zPos);
	//Chunk* getChunkNoOffset(int xPos, int zPos);

	inline bool fileExists(const std::string& name);

	std::shared_ptr<renderer::Renderer> renderer;

	std::shared_ptr<world::Area> area;
};

} // namespace world
