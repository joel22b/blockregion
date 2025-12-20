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

class World {
public:
	World();
	~World();

	errors::expected<> loadArea(ChunkCoord coord);

	Block* getBlock(GridCoord blockCoord);

private:
	std::shared_ptr<renderer::Wrapper<Chunk>> getChunk(ChunkCoord coord);

	std::shared_ptr<world::Area> area;
	std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace world
