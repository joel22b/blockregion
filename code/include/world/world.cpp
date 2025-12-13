#include "world/world.h"

#include <spdlog/spdlog.h>

#include <world/generation/chunk.h>

namespace world
{

World::World(std::shared_ptr<renderer::Renderer> _renderer)
{
	m_logger = spdlog::get("blockregion");
	
	renderer = _renderer;
}

World::~World()
{}

errors::expected<>
World::loadArea(ChunkCoord coord)
{
	if ((!area) || (*area != coord))
	{
		m_logger->debug("Loading new area: {}", coord);

		// Remove old area
		area = nullptr;

		// Create new area
		area = std::make_shared<world::Area>(std::bind(&world::World::getChunk, this, std::placeholders::_1), coord, 3);
	}

	return {};
}

Block*
World::getBlock(GridCoord blockCoord)
{
	std::shared_ptr<renderer::Wrapper<Chunk>> chunkWrapper = (*area)[blockCoord];
	if (chunkWrapper == nullptr)
	{
		return nullptr;
	}
	return (*chunkWrapper)->getBlock(blockCoord);
}

std::shared_ptr<renderer::Wrapper<Chunk>>
World::getChunk(ChunkCoord coord)
{
	// Currently only generate chunks
	errors::expected<std::shared_ptr<Chunk>> chunk = generation::generateChunk(coord);
	if (errors::has_error(chunk))
	{
		m_logger->error("Failed to generate chunk {}: {}", coord, chunk.error());
		return nullptr;
	}

	std::shared_ptr<renderer::Wrapper<Chunk>> wrapper = std::make_shared<renderer::Wrapper<Chunk>>(renderer, chunk.value());
	return wrapper;
}

} // namespace world
