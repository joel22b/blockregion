#include "world.h"

//#include "../utils/Logger.h"
//#define LOG(severity, msg) Logger::log("World.cpp", severity, msg)

World::World() {}

World::World(std::shared_ptr<renderer::Renderer> _renderer) {
	blockConsts = new Block_Consts();
	chunkXOffset = 0;
	chunkZOffset = 0;
	this->chunks = nullptr;
	renderer = _renderer;
}

World::~World() {
	//LOG(INFO, "Deleting");
}

void World::doUpdate() {
	
}

void World::doRender(GLint modelLoc) {
	/*if (chunksMutex.try_lock()) {
		for (int i = 0; i < chunksLength * chunksLength; i++) {
			if (chunks[i] != nullptr) {
				//std::ostringstream msg;
				//msg << "Rendering chunk " << i << " if possible";
				//LOG(DEBUG, msg.str());
				if (chunks[i]->chunk->shouldRender()) {
					chunks[i]->doRender(modelLoc);
				}
			}
			else {
				//std::ostringstream msg;
				//msg << "Cannot render i=" << i;
				//LOG(WARN, msg.str());
			}
		}
		chunksMutex.unlock();
	}*/
}

Chunk* World::getChunk(int xPos, int zPos) {
	int x = xPos + chunkXOffset;
	int z = zPos + chunkZOffset;

	if (x < 0 || x >= chunksLength || z < 0 || z >= chunksLength) {
		return nullptr;
	}
	return chunks[(x * chunksLength) + z]->chunk;
}

Chunk* World::getChunkNoOffset(int xPos, int zPos) {
	if (xPos < 0 || xPos >= chunksLength || zPos < 0 || zPos >= chunksLength) {
		return nullptr;
	}

	return chunks[(xPos * chunksLength) + zPos]->chunk;
}

Chunk* World::getChunkByCoords(int xPos, int zPos) {
	int x = (xPos >= 0) ? xPos / CHUNK_MAX_WIDTH : (xPos / CHUNK_MAX_WIDTH) - 1;
	int z = (zPos >= 0) ? zPos / CHUNK_MAX_WIDTH : (zPos / CHUNK_MAX_WIDTH) - 1;
	return getChunk(x, z);
}

glm::vec2 World::getChunkCoords(int xPos, int zPos) {
	int x = (xPos >= 0) ? xPos / CHUNK_MAX_WIDTH : (xPos / CHUNK_MAX_WIDTH) - 1;
	int z = (zPos >= 0) ? zPos / CHUNK_MAX_WIDTH : (zPos / CHUNK_MAX_WIDTH) - 1;
	return glm::vec2(x, z);
}

Block* World::getBlock(int xPos, int yPos, int zPos) {
	Chunk* chunk = getChunkByCoords(xPos, zPos);
	if (chunk == nullptr || yPos < 0 || yPos > CHUNK_MAX_HEIGHT) {
		return nullptr;
	}
	return chunk->getBlock((xPos >= 0 || xPos % CHUNK_MAX_WIDTH == 0) ? xPos % CHUNK_MAX_WIDTH : (xPos % CHUNK_MAX_WIDTH) + CHUNK_MAX_WIDTH,
		yPos, (zPos >= 0 || zPos % CHUNK_MAX_WIDTH == 0) ? zPos % CHUNK_MAX_WIDTH : (zPos % CHUNK_MAX_WIDTH) + CHUNK_MAX_WIDTH);
}

std::string World::getWorldFolder() {
	return "saves\\" + worldName + "\\";
}

void World::generateChunk(Chunk* chunk) {
	for (int x = 0; x < CHUNK_MAX_WIDTH; x++) {
		for (int z = 0; z < CHUNK_MAX_WIDTH; z++) {
			// Get base value (Between -1 and 1)
			float baseValue = glm::simplex(glm::vec2((chunk->getXPos() + x) / 50.0f, (chunk->getZPos() + z) / 50.0f));
			// Make base value between 0 and 10
			baseValue = (baseValue + 1) * 5;

			// Get height value (Between -1 and 1)
			float heightValue = glm::simplex(glm::vec2((chunk->getXPos() + x) / 24.0f, (chunk->getZPos() + z)/ 24.0f));
			// Make height value between 0 and 2
			heightValue = heightValue + 1;

			int y = roundf(heightValue * baseValue);

			chunk->addBlock(glm::vec3(x, y, z), GRASS);
			for (int i = 0; i < y; i++) {
				chunk->addBlock(glm::vec3(x, i, z), DIRT);
			}
		}
	}
}

void World::loadChunk(Chunk* chunk) {
	// Check if chunk file exists
	const std::string chunkFilePath = getWorldFolder() + "chunks\\x" + std::to_string(chunk->getChunkXPos()) + "z" + std::to_string(chunk->getChunkZPos()) + ".chunk";
	if (fileExists(chunkFilePath)) {
		// Load in chunk from file
		std::string fileData;
		std::ifstream file;
		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::badbit);
		try {
			// Open files
			file.open(chunkFilePath);
			std::stringstream stream;
			// Read file's buffer contents into streams
			stream << file.rdbuf();
			// Close file handlers
			file.close();
			// Convert stream into string
			fileData = stream.str();
		}
		catch (std::ifstream::failure e) {
			//LOG(ERROR, "File not able to be successfully read in loadChunk: \"" + chunkFilePath + "\"");
			return;
		}

		// Check that fileData is valid
		if (fileData.length() != CHUNK_MAX_WIDTH * CHUNK_MAX_WIDTH * CHUNK_MAX_HEIGHT) {
			//std::ostringstream msg;
			//msg << "File size incorrect in loadChunk: \"" << chunkFilePath << "\" size=" << fileData.length();
			//LOG(ERROR, msg.str());
			return;
		}

		// Load fileData into chunk
		int count = 0;
		for (int y = 0; y < CHUNK_MAX_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_MAX_WIDTH; z++) {
				for (int x = 0; x < CHUNK_MAX_WIDTH; x++) {
					chunk->addBlock(x, y, z, Block_Type(int(fileData.at(count++)) - 33));
				}
			}
		}
	}
	else {
		// Generate new chunk
		generateChunk(chunk);
		saveChunk(chunk);
	}
}

void World::saveChunk(Chunk* chunk) {
	const std::string chunkFilePath = getWorldFolder() + "chunks\\x" + std::to_string(chunk->getChunkXPos()) + "z" + std::to_string(chunk->getChunkZPos()) + ".chunk";

	// Put all chunk data into a string
	std::string chunkData = "";
	for (int y = 0; y < CHUNK_MAX_HEIGHT; y++) {
		for (int z = 0; z < CHUNK_MAX_WIDTH; z++) {
			for (int x = 0; x < CHUNK_MAX_WIDTH; x++) {
				Block* block = chunk->getBlock(x, y, z);
				char blockData;
				if (block == nullptr) {
					blockData = 33;
				}
				else {
					blockData = block->getType() + 33;
				}
				chunkData.push_back(blockData);
			}
		}
	}

	try {
		std::ofstream file(chunkFilePath);
		file << chunkData;
		file.close();
	}
	catch (std::ofstream::failure e) {
		//LOG(ERROR, "File not able to be successfully written in saveChunk: \"" + chunkFilePath + "\"");
		return;
	}
}

inline bool World::fileExists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void World::updateChunkNoOffset(int xPos, int zPos) {
	Chunk* chunk = getChunkNoOffset(xPos, zPos);

	if (chunk == nullptr) {
		std::ostringstream msg;
		msg << "Failed to update chunk x=" << xPos << " z=" << zPos;
		//LOG(WARN, msg.str());
		return;
	}

	ChunkData* cData = chunks[(xPos * chunksLength) + zPos];

	if (cData->renderId == 0)
	{
		errors::expected<renderer::RenderId> renderId = renderer->registerNew(chunk);
		if (errors::has_error(renderId))
		{
			std::cout << "Shift chunks: " << renderId.error() << std::endl;
		}

		cData->renderId = renderId.value();
	}
	else
	{
		errors::expected<> ret = renderer->registerExisting(cData->renderId, chunk);
		if (errors::has_error(ret))
		{
			std::cout << "Shift chunks: " << ret.error() << std::endl;
		}
	}
}

void World::updateChunkNoOffset(Chunk* chunk) {
	int xPos = chunk->getChunkXPos();
	int zPos = chunk->getChunkZPos();

	if (chunk == nullptr) {
		std::ostringstream msg;
		msg << "Failed to update chunk x=" << xPos << " z=" << zPos;
		//LOG(WARN, msg.str());
		return;
	}

	ChunkData* cData = chunks[(xPos * chunksLength) + zPos];

	if (cData->renderId == 0)
	{
		errors::expected<renderer::RenderId> renderId = renderer->registerNew(chunk);
		if (errors::has_error(renderId))
		{
			std::cout << "Shift chunks: " << renderId.error() << std::endl;
		}

		cData->renderId = renderId.value();
	}
	else
	{
		errors::expected<> ret = renderer->registerExisting(cData->renderId, chunk);
		if (errors::has_error(ret))
		{
			std::cout << "Shift chunks: " << ret.error() << std::endl;
		}
	}
}

void World::shiftChunksThread(Block_Consts* blockConsts, int xPos, int zPos) {
	shiftMutex.lock();
	int prevXPos = (renderDistance + bufferDistance) - chunkXOffset;
	int prevZPos = (renderDistance + bufferDistance) - chunkZOffset;
	int chunkXOffsetNew = chunkXOffset;
	int chunkZOffsetNew = chunkZOffset;

	// Copy chunks array
	ChunkData** chunksTemp = new ChunkData * [chunksLength * chunksLength];
	for (int i = 0; i < chunksLength * chunksLength; i++) {
		chunksTemp[i] = chunks[i];
	}

	// Check X
	if (xPos != prevXPos) {
		int shiftDist = (prevXPos - xPos) * chunksLength;
		chunkXOffsetNew += prevXPos - xPos;

		if (xPos > prevXPos) {
			// Shift and clean chunks array
			for (int i = 0; i < chunksLength * chunksLength; i++) {
				ChunkData* chunk = chunksTemp[i];
				chunksTemp[i] = nullptr;

				if (i + shiftDist < 0) {
					saveChunk(chunk->chunk);
					chunk->chunk->setDelete();

					//std::ostringstream msg;
					//msg << "Marking chunk for delete c1 " << i;
					//LOG(DEBUG, msg.str());
				}
				else {
					chunksTemp[i + shiftDist] = chunk;
				}
			}

			// Load new data
			for (int i = (chunksLength * chunksLength) + shiftDist; i < chunksLength * chunksLength; i++) {
				if (chunksTemp[i] != nullptr) {
					//std::ostringstream msg;
					//msg << "i=" << i << " shiftDist=" << shiftDist << " chunksLength=" << chunksLength;
					//LOG(INFO, msg.str());
				}
				else {
					int chunkX = (int)(i / chunksLength) - chunkXOffsetNew;
					int chunkZ = i % chunksLength - chunkZOffsetNew;
					chunksTemp[i] = new ChunkData();
					chunksTemp[i]->chunk = new Chunk(blockConsts, chunkX, chunkZ);
					loadChunk(chunksTemp[i]->chunk);
				}
			}
		}
		else {
			// Shift and clean chunks array
			for (int i = (chunksLength * chunksLength) - 1; i >= 0; i--) {
				ChunkData* chunk = chunksTemp[i];
				chunksTemp[i] = nullptr;

				if (i + shiftDist >= chunksLength * chunksLength) {
					saveChunk(chunk->chunk);
					chunk->chunk->setDelete();

					//std::ostringstream msg;
					//msg << "Marking chunk for delete c2 " << i;
					//LOG(DEBUG, msg.str());
				}
				else {
					chunksTemp[i + shiftDist] = chunk;
				}
			}

			// Load new data
			for (int i = 0; i < shiftDist; i++) {
				if (chunksTemp[i] != nullptr) {
					//std::ostringstream msg;
					//msg << "i=" << i << " shiftDist=" << shiftDist << " chunksLength=" << chunksLength;
					//LOG(INFO, msg.str());
				}
				else {
					int chunkX = (int)(i / chunksLength) - chunkXOffsetNew;
					int chunkZ = i % chunksLength - chunkZOffsetNew;
					chunksTemp[i] = new ChunkData();
					chunksTemp[i]->chunk = new Chunk(blockConsts, chunkX, chunkZ);
					loadChunk(chunksTemp[i]->chunk);
				}
			}
		}
	}

	// Check Z
	if (zPos != prevZPos) {
		int shiftDist = prevZPos - zPos;
		chunkZOffsetNew += shiftDist;

		if (zPos > prevZPos) {
			// Shift and clean chunks array
			for (int i = 0; i < chunksLength * chunksLength; i++) {
				ChunkData* chunk = chunksTemp[i];
				chunksTemp[i] = nullptr;

				if ((int)((i + shiftDist) / chunksLength) != (int)(i / chunksLength) || i + shiftDist < 0) {
					saveChunk(chunk->chunk);
					chunk->chunk->setDelete();

					//std::ostringstream msg;
					//msg << "Marking chunk for delete c3 " << i;
					//LOG(DEBUG, msg.str());
				}
				else {
					chunksTemp[i + shiftDist] = chunk;
				}
			}
			
			// Load new chunks
			for (int i = 0; i < chunksLength * chunksLength; i++) {
				if (i % chunksLength >= chunksLength + shiftDist) {
					if (chunksTemp[i] != nullptr) {
						//std::ostringstream msg;
						//msg << "i=" << i << " shiftDist=" << shiftDist << " chunksLength=" << chunksLength;
						//LOG(INFO, msg.str());
					}
					int chunkX = (int)(i / chunksLength) - chunkXOffsetNew;
					int chunkZ = i % chunksLength - chunkZOffsetNew;
					ChunkData* chunk = new ChunkData();
					chunk->chunk = new Chunk(blockConsts, chunkX, chunkZ);
					loadChunk(chunk->chunk);
					chunksTemp[i] = chunk;
				}
			}
		}
		else {
			// Shift and clean chunks array
			for (int i = (chunksLength * chunksLength) - 1; i >= 0 ; i--) {
				ChunkData* chunk = chunksTemp[i];
				chunksTemp[i] = nullptr;

				if ((int)((i + shiftDist) / chunksLength) != (int)(i / chunksLength) || i + shiftDist >= chunksLength * chunksLength) {
					saveChunk(chunk->chunk);
					chunk->chunk->setDelete();

					//std::ostringstream msg;
					//msg << "Marking chunk for delete c4 " << i;
					//LOG(DEBUG, msg.str());
				}
				else {
					chunksTemp[i + shiftDist] = chunk;
				}
			}

			// Load new chunks
			for (int i = 0; i < chunksLength * chunksLength; i++) {
				if (i % chunksLength < shiftDist) {
					if (chunksTemp[i] != nullptr) {
						//std::ostringstream msg;
						//msg << "i=" << i << " shiftDist=" << shiftDist << " chunksLength=" << chunksLength;
						//LOG(INFO, msg.str());
					}
					int chunkX = (int)(i / chunksLength) - chunkXOffsetNew;
					int chunkZ = i % chunksLength - chunkZOffsetNew;
					ChunkData* chunk = new ChunkData();
					chunk->chunk = new Chunk(blockConsts, chunkX, chunkZ);
					loadChunk(chunk->chunk);
					chunksTemp[i] = chunk;
				}
			}
		}
	}

	// Update the chunks
	for (int x = 0; x < chunksLength; x++) {
		for (int z = 0; z < chunksLength; z++) {
			if (x >= bufferDistance && x < chunksLength - bufferDistance && z >= bufferDistance && z < chunksLength - bufferDistance) {
				// These are chunks to be rendered
				ChunkData* chunk = chunksTemp[(x * chunksLength) + z];

				if (chunk->renderId == 0)
				{
					errors::expected<renderer::RenderId> renderId = renderer->registerNew(chunk->chunk);
					if (errors::has_error(renderId))
					{
						std::cout << "Shift chunks: " << renderId.error() << std::endl;
					}

					chunk->renderId = renderId.value();
				}
				else
				{
					errors::expected<> ret = renderer->registerExisting(chunk->renderId, chunk->chunk);
					if (errors::has_error(ret))
					{
						std::cout << "Shift chunks: " << ret.error() << std::endl;
					}
				}
			}
			else {
				// These are buffer chunks
			}
		}
	}

	//LOG(DEBUG, "Done generating new chunks array, entering crtical section");

	// Switch to new chunks array
	chunksMutex.lock();
	ChunkData** chunksOld = chunks;
	chunks = chunksTemp;
	chunkXOffset = chunkXOffsetNew;
	chunkZOffset = chunkZOffsetNew;
	chunksMutex.unlock();

	//LOG(DEBUG, "Exited critical section, removing old");

	// Clean up old chunks array
	for (int i = 0; i < chunksLength * chunksLength; i++) {
		if (chunksOld[i]->chunk->shouldDelete()) {
			delete chunksOld[i];
		}
	}
	delete[] chunksOld;

	//LOG(DEBUG, "Chunk shifter thread done");
	shiftMutex.unlock();
}

void World::shiftChunks(int xPos, int zPos) {
	//LOG(DEBUG, "Shifting chunks");

	//std::thread threadShiftChunks(&World::shiftChunksThread, this, blockConsts, xPos, zPos);
	//threadShiftChunks.detach();

	shiftChunksThread(blockConsts, xPos, zPos);
}

void World::updateChunkRenderDistance(int renderDistance, int bufferDistance, int xPos, int zPos) {
	this->renderDistance = renderDistance;
	this->bufferDistance = bufferDistance;

	// Clear chunk arrays
	if (chunks != nullptr) {
		for (int i = 0; i < chunksLength * chunksLength; i++) {
			saveChunk(chunks[i]->chunk);
			delete chunks[i];
		}
		delete[] chunks;
	}

	// Create new arrays
	chunksLength = 1 + (renderDistance * 2) + (bufferDistance * 2);
	chunks = new ChunkData * [chunksLength * chunksLength];
	
	// Set offsets
	int chunkX = (xPos >= 0) ? xPos / CHUNK_MAX_WIDTH : (xPos / CHUNK_MAX_WIDTH) - 1;
	int chunkZ = (zPos >= 0) ? zPos / CHUNK_MAX_WIDTH : (zPos / CHUNK_MAX_WIDTH) - 1;

	chunkXOffset = chunkX + renderDistance + bufferDistance;
	chunkZOffset = chunkZ + renderDistance + bufferDistance;

	// Load chunks
	for (int x = -(renderDistance + bufferDistance); x <= renderDistance + bufferDistance; x++) {
		for (int z = -(renderDistance + bufferDistance); z <= renderDistance + bufferDistance; z++) {
			int offsetX = x + chunkXOffset;
			int offsetZ = z + chunkZOffset;
			chunks[(offsetX * chunksLength) + offsetZ] = new ChunkData();
			chunks[(offsetX * chunksLength) + offsetZ]->chunk = new Chunk(blockConsts, x, z);
			loadChunk(chunks[(offsetX * chunksLength) + offsetZ]->chunk);
		}
	}

	// Update the chunks
	for (int i = bufferDistance; i <= (renderDistance * 2) + bufferDistance; i++) {
		for (int j = bufferDistance; j <= (renderDistance * 2) + bufferDistance; j++) {
			updateChunkNoOffset(i, j);
		}
	}
}