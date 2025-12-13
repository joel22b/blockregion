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

    for (int x = 0; x < consts::CHUNK_MAX_WIDTH; x++)
	{
		for (int z = 0; z < consts::CHUNK_MAX_WIDTH; z++)
		{
			GridCoord blockCoord(gridCoord.x + x, gridCoord.y, gridCoord.z + z);
			// Get base value (Between -1 and 1)
			float baseValue = glm::simplex(glm::vec2(blockCoord.x / 50.0f, blockCoord.z / 50.0f));
			// Make base value between 0 and 10
			baseValue = (baseValue + 1) * 5;

			// Get height value (Between -1 and 1)
			float heightValue = glm::simplex(glm::vec2(blockCoord.x / 24.0f, blockCoord.z / 24.0f));
			// Make height value between 0 and 2
			heightValue = heightValue + 1;

			blockCoord.y = roundf(heightValue * baseValue);

			chunk->addBlock(blockCoord, GRASS);
			for (blockCoord.y--; blockCoord.y >= 0 ; blockCoord.y--) {
				chunk->addBlock(blockCoord, DIRT);
			}
		}
	}

    return chunk;
}

} // namespace generation

} // namespace world
