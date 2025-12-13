#pragma once

#include <memory>

#include <errors/br-expected.h>
#include <world/chunk.h>

namespace world
{

namespace generation
{

inline
errors::expected<std::shared_ptr<Chunk>>
generateChunk(ChunkCoord coord)
{
    std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(coord);
    GridCoord gridCoord(coord);

    for (int x = 0; x < consts::CHUNK_MAX_WIDTH; x++) {
		for (int z = 0; z < consts::CHUNK_MAX_WIDTH; z++) {
			// Get base value (Between -1 and 1)
			float baseValue = glm::simplex(glm::vec2((gridCoord.x + x) / 50.0f, (gridCoord.z + z) / 50.0f));
			// Make base value between 0 and 10
			baseValue = (baseValue + 1) * 5;

			// Get height value (Between -1 and 1)
			float heightValue = glm::simplex(glm::vec2((gridCoord.x + x) / 24.0f, (gridCoord.z + z)/ 24.0f));
			// Make height value between 0 and 2
			heightValue = heightValue + 1;

			int y = roundf(heightValue * baseValue);

			chunk->addBlock(glm::vec3(x, y, z), GRASS);
			for (int i = 0; i < y; i++) {
				chunk->addBlock(glm::vec3(x, i, z), DIRT);
			}
		}
	}

    return chunk;
}

} // namespace generation

} // namespace world
