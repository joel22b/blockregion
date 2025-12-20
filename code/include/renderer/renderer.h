#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <vector>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"

#include <spdlog/spdlog.h>

#include <renderer/types.h>
#include <renderer/window.h>
#include <errors/br-expected.h>
#include <world/types.h>


namespace renderer
{

using KeyboardCallback = std::function<void(int, int, int, int)>;
using MouseCallback = std::function<void(double, double)>;

class Renderer
{
public:
    Renderer();

    std::shared_ptr<Window> getWindow() { return window; }

    void updateFOV(float fov);
    void updateCamera(glm::mat4 viewMatrix, world::Coord position);

    void renderAll();

    template<typename T>
    errors::expected<RenderId> registerNew(T toRegister)
    {
        errors::expected<std::shared_ptr<MeshTypes>> newMesh = generateMesh(toRegister);
        if (errors::has_error(newMesh))
        {
            return std::unexpected(newMesh.error().prefix("Renderer: registerNew: "));
        }

        RenderId id = getNewId();
        meshes.emplace(id, newMesh.value());
        return id;
    }

    template<typename T>
    errors::expected<> registerExisting(RenderId id, T toRegister)
    {
        if (id == 0)
        {
            return errors::unexpected("Renderer: registerExisting: 0 is invalid Id", errors::Code::InvalidArgument);
        }
        if (!meshes.contains(id))
        {
            return errors::unexpected("Renderer: registerExisting: no mesh with Id="+std::to_string(id), errors::Code::NotFound);
        }

        errors::expected<std::shared_ptr<MeshTypes>> newMesh = generateMesh(toRegister);
        if (errors::has_error(newMesh))
        {
            return std::unexpected(newMesh.error().prefix("Renderer: registerExisting: "));
        }

        meshes[id] = newMesh.value();
        return {};
    }

    errors::expected<> unregister(RenderId id);

private:
    template<typename T>
    errors::expected<std::shared_ptr<MeshTypes>> generateMesh(T pre);

    RenderId getNewId();

    std::shared_ptr<Window> window;

    std::shared_ptr<textures::Loader> texLoader;

    // Shaders
	std::shared_ptr<shaders::Block> blockShader;
    std::shared_ptr<shaders::Text> textShader;

    std::map<RenderId, std::shared_ptr<MeshTypes>> meshes;

    RenderId nextId {1};

    std::shared_ptr<spdlog::logger> m_logger;
};

// Meyer's Singleton design for thread safe and lazy initialization
inline
std::shared_ptr<Renderer>
getGlobalRenderer()
{
    static auto instance = std::make_shared<Renderer>();
    return instance;
}

} // namespace renderer

#include <world/chunk.h>
#include <text/text.h>

namespace renderer
{

template<>
inline
errors::expected<std::shared_ptr<MeshTypes>>
Renderer::generateMesh(std::shared_ptr<world::Chunk> pre)
{
    std::vector<Block_Face> blockFaces;
	Block_Face blockFace;

    textures::TextureSet texSet = texLoader->getTextureSet("blocks");

	for (int x = 0; x < world::consts::CHUNK_MAX_WIDTH; x++) {
		for (int y = 0; y < world::consts::CHUNK_MAX_HEIGHT; y++) {
			for (int z = 0; z < world::consts::CHUNK_MAX_WIDTH; z++) {
				if (pre->getBlock(world::ChunkInternalCoord(x, y, z))->getType() == world::AIR) {
					// Add surrounding faces

					// XPOS
					if (x != 0) {
						if (pre->getBlock(world::ChunkInternalCoord(x - 1, y, z))->getType() != world::AIR) {
							blockFace.Position = glm::vec3(x, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(1, 0, 0);
                            blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(x - 1, y, z))->getType(), world::XPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// XNEG
					if (x != world::consts::CHUNK_MAX_WIDTH - 1) {
						if (pre->getBlock(world::ChunkInternalCoord(x + 1, y, z))->getType() != world::AIR) {
							blockFace.Position = glm::vec3(x + 1, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(-1, 0, 0);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(x + 1, y, z))->getType(), world::XNEG);
							blockFaces.push_back(blockFace);
						}
					}
					// YPOS
					if (y != 0) {
						if (pre->getBlock(world::ChunkInternalCoord(x, y - 1, z))->getType() != world::AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y, z + 0.5f);
							blockFace.Normal = glm::vec3(0, 1, 0);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(x, y - 1, z))->getType(), world::YPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// YNEG
					if (y != world::consts::CHUNK_MAX_HEIGHT - 1) {
						if (pre->getBlock(world::ChunkInternalCoord(x, y + 1, z))->getType() != world::AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 1, z + 0.5f);
							blockFace.Normal = glm::vec3(0, -1, 0);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(x, y + 1, z))->getType(), world::YNEG);
							blockFaces.push_back(blockFace);
						}
					}
					// ZPOS
					if (z != 0) {
						if (pre->getBlock(world::ChunkInternalCoord(x, y, z - 1))->getType() != world::AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z);
							blockFace.Normal = glm::vec3(0, 0, 1);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(x, y, z - 1))->getType(), world::ZPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// ZNEG
					if (z != world::consts::CHUNK_MAX_WIDTH - 1) {
						if (pre->getBlock(world::ChunkInternalCoord(x, y, z + 1))->getType() != world::AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z + 1);
							blockFace.Normal = glm::vec3(0, 0, -1);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(x, y, z + 1))->getType(), world::ZNEG);
							blockFaces.push_back(blockFace);
						}
					}
				}
			}
		}
	}

    // TODO: Do properly
    // Temp fix for chunk borders: Assume AIR if outside curr chunk
    for (int w = 0; w < world::consts::CHUNK_MAX_WIDTH; w++)
    {
        for (int y = 0; y < world::consts::CHUNK_MAX_HEIGHT; y++)
        {
            // XPOS
            if (pre->getBlock(world::ChunkInternalCoord(world::consts::CHUNK_MAX_WIDTH-1, y, w))->getType() != world::AIR)
            {
                blockFace.Position = glm::vec3(world::consts::CHUNK_MAX_WIDTH, y + 0.5f, w + 0.5f);
                blockFace.Normal = glm::vec3(1, 0, 0);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(world::consts::CHUNK_MAX_WIDTH-1, y, w))->getType(), world::XPOS);
                blockFaces.push_back(blockFace);
            }
            // XNEG
            if (pre->getBlock(world::ChunkInternalCoord(0, y, w))->getType() != world::AIR)
            {
                blockFace.Position = glm::vec3(0, y + 0.5f, w + 0.5f);
                blockFace.Normal = glm::vec3(-1, 0, 0);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(0, y, w))->getType(), world::XNEG);
                blockFaces.push_back(blockFace);
            }
            // ZPOS
            if (pre->getBlock(world::ChunkInternalCoord(w, y, world::consts::CHUNK_MAX_WIDTH-1))->getType() != world::AIR)
            {
                blockFace.Position = glm::vec3(w + 0.5f, y + 0.5f, world::consts::CHUNK_MAX_WIDTH);
                blockFace.Normal = glm::vec3(0, 0, 1);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(w, y, world::consts::CHUNK_MAX_WIDTH-1))->getType(), world::ZPOS);
                blockFaces.push_back(blockFace);
            }
            // ZNEG
            if (pre->getBlock(world::ChunkInternalCoord(w, y, 0))->getType() != world::AIR)
            {
                blockFace.Position = glm::vec3(w + 0.5f, y + 0.5f, 0);
                blockFace.Normal = glm::vec3(0, 0, -1);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(world::ChunkInternalCoord(w, y, 0))->getType(), world::ZNEG);
                blockFaces.push_back(blockFace);
            }
        }
    }

    world::GridCoord chunkOffset(pre->getCoord());

    std::shared_ptr<MeshTypes> newMesh = std::make_shared<MeshTypes>(Chunk_Mesh(blockFaces, glGetUniformLocation(blockShader->getProgram(), "model"),
        glm::translate(glm::mat4(1), chunkOffset.getVec()), false));

    struct MeshVisitor
    {
        std::shared_ptr<shaders::Block> blockShader;

        void operator()(Chunk_Mesh& cMesh) const { cMesh.setShader(blockShader); }
        void operator()(meshes::Text& mesh) const {}
    };

    MeshVisitor mv;
    mv.blockShader = blockShader;
    std::visit(mv, *newMesh);

    return newMesh;
}

template<>
inline
errors::expected<std::shared_ptr<MeshTypes>>
Renderer::generateMesh(text::Text* text)
{
    std::shared_ptr<MeshTypes> newMesh = std::make_shared<MeshTypes>(meshes::Text(textShader, text->text, text->x, text->y, text->scale, text->color));
    return newMesh;
}

} // namespace renderer
