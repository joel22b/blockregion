#include "chunk.h"

//#include "../utils/Logger.h"
//#define LOG(severity, msg) Logger::log("Chunk.cpp", severity, msg)

Chunk::Chunk() {
	this->render = false;
}

Chunk::Chunk(Block_Consts* blockConsts, int xPos, int zPos) {
	blocks = std::vector<std::vector<std::vector<Block>>>(CHUNK_MAX_WIDTH, std::vector<std::vector<Block>>(CHUNK_MAX_HEIGHT, std::vector<Block>(CHUNK_MAX_WIDTH, Block())));
	this->blockConsts = blockConsts;
	this->xPos = xPos;
	this->zPos = zPos;
	this->render = false;
	this->toDelete = false;
}

Chunk::~Chunk() {
	//LOG(DEBUG, "Deleting");
	if (chunkMesh != nullptr) {
		delete chunkMesh;
	}
}

void Chunk::doUpdate(Chunk* chunkXPOS, Chunk* chunkXNEG, Chunk* chunkZPOS, Chunk* chunkZNEG) {
	std::vector<Texture> blockTextures = blockConsts->getBlockTextures();
	std::vector<Block_Face> blockFaces = calculateMesh(chunkXPOS, chunkXNEG, chunkZPOS, chunkZNEG);

	chuckMeshMutex.lock();
	this->render = false;
	Chunk_Mesh* newChunkMesh = new Chunk_Mesh(blockFaces, blockTextures);
	if (chunkMesh != nullptr) {
		Chunk_Mesh* oldChunkMesh = chunkMesh;
		chunkMesh = newChunkMesh;
		delete oldChunkMesh;
	}
	else {
		chunkMesh = newChunkMesh;
	}
	this->render = true;
	chuckMeshMutex.unlock();
}

void Chunk::doPartialUpdate(Chunk* chunkXPOS, Chunk* chunkXNEG, Chunk* chunkZPOS, Chunk* chunkZNEG) {
	std::vector<Block_Face> blockFaces = calculateMesh(chunkXPOS, chunkXNEG, chunkZPOS, chunkZNEG);

	chuckMeshMutex.lock();
	this->render = false;
	Chunk_Mesh* newChunkMesh = new Chunk_Mesh(blockFaces);
	if (chunkMesh != nullptr) {
		Chunk_Mesh* oldChunkMesh = chunkMesh;
		chunkMesh = newChunkMesh;
		delete oldChunkMesh;
	}
	else {
		chunkMesh = newChunkMesh;
	}
	this->render = true;
	chuckMeshMutex.unlock();
}

std::vector<Block_Face> Chunk::calculateMesh(Chunk* chunkXPOS, Chunk* chunkXNEG, Chunk* chunkZPOS, Chunk* chunkZNEG) {
	std::vector<Block_Face> blockFaces;
	Block_Face blockFace;

	for (int x = 0; x < CHUNK_MAX_WIDTH; x++) {
		for (int y = 0; y < CHUNK_MAX_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_MAX_WIDTH; z++) {
				if (blocks[x][y][z].getType() == AIR) {
					// Add surrounding faces

					// XPOS
					if (x != 0) {
						if (blocks[x - 1][y][z].getType() != AIR) {
							blockFace.Position = glm::vec3(x, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(1, 0, 0);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(blocks[x - 1][y][z].getType(), XPOS);
							blockFaces.push_back(blockFace);
						}
					}
					else if (chunkXNEG != nullptr) {
						if (chunkXNEG->getBlock(CHUNK_MAX_WIDTH - 1, y, z)->getType() != AIR) {
							blockFace.Position = glm::vec3(x, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(1, 0, 0);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(chunkXNEG->getBlock(CHUNK_MAX_WIDTH - 1, y, z)->getType(), XPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// XNEG
					if (x != CHUNK_MAX_WIDTH - 1) {
						if (blocks[x + 1][y][z].getType() != AIR) {
							blockFace.Position = glm::vec3(x + 1, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(-1, 0, 0);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(blocks[x + 1][y][z].getType(), XNEG);
							blockFaces.push_back(blockFace);
						}
					}
					else if (chunkXPOS != nullptr) {
						if (chunkXPOS->getBlock(0, y, z)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 1, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(-1, 0, 0);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(chunkXPOS->getBlock(0, y, z)->getType(), XNEG);
							blockFaces.push_back(blockFace);
						}
					}
					// YPOS
					if (y != 0) {
						if (blocks[x][y - 1][z].getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y, z + 0.5f);
							blockFace.Normal = glm::vec3(0, 1, 0);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(blocks[x][y - 1][z].getType(), YPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// YNEG
					if (y != CHUNK_MAX_HEIGHT - 1) {
						if (blocks[x][y + 1][z].getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 1, z + 0.5f);
							blockFace.Normal = glm::vec3(0, -1, 0);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(blocks[x][y + 1][z].getType(), YNEG);
							blockFaces.push_back(blockFace);
						}
					}
					// ZPOS
					if (z != 0) {
						if (blocks[x][y][z - 1].getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z);
							blockFace.Normal = glm::vec3(0, 0, 1);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(blocks[x][y][z - 1].getType(), ZPOS);
							blockFaces.push_back(blockFace);
						}
					}
					else if (chunkZNEG != nullptr) {
						if (chunkZNEG->getBlock(x, y, CHUNK_MAX_WIDTH - 1)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z);
							blockFace.Normal = glm::vec3(0, 0, 1);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(chunkZNEG->getBlock(x, y, CHUNK_MAX_WIDTH - 1)->getType(), ZPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// ZNEG
					if (z != CHUNK_MAX_WIDTH - 1) {
						if (blocks[x][y][z + 1].getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z + 1);
							blockFace.Normal = glm::vec3(0, 0, -1);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(blocks[x][y][z + 1].getType(), ZNEG);
							blockFaces.push_back(blockFace);
						}
					}
					else if (chunkZPOS != nullptr) {
						if (chunkZPOS->getBlock(x, y, 0)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z + 1);
							blockFace.Normal = glm::vec3(0, 0, -1);
							blockFace.TexCoords = blockConsts->getBlockTexCoords(chunkZPOS->getBlock(x, y, 0)->getType(), ZNEG);
							blockFaces.push_back(blockFace);
						}
					}
				}
			}
		}
	}

	return blockFaces;
}

Chunk_Mesh* Chunk::getChunkMesh() {
	return chunkMesh;
}

void Chunk::doRender(Shader shader, GLuint modelLoc) {
	chuckMeshMutex.lock();
	if (chunkMesh != nullptr) {
		if (!chunkMesh->ready()) {
			chunkMesh->doSetup(blockConsts->getBlockTextures());
		}
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1), glm::vec3(xPos * CHUNK_MAX_WIDTH, 0, zPos * CHUNK_MAX_WIDTH))));
		chunkMesh->doRender(shader);
	}
	else {
		//std::ostringstream msg;
		//msg << "No chunk mesh x=" << xPos << " z=" << zPos;
		//LOG(WARN, msg.str());
	}
	chuckMeshMutex.unlock();
}

bool Chunk::shouldRender() {
	//chuckMeshMutex.lock();
	return this->render && chunkMesh != nullptr;
	//chuckMeshMutex.unlock();
}

void Chunk::setRender(bool render) {
	this->render = render;
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