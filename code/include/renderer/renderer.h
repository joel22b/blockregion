#pragma once

#include <variant>
#include <memory>
#include <map>

#include <errors/br-expected.h>

#include <textures/loader.h>

// List all shaders
#include <shaders/block.h>

// List all meshes
#include "code/src/chunk-mesh.h"

#include "code/src/chunk.h"

namespace renderer
{

using RenderId = uint32_t;
using MeshTypes = std::variant<Chunk_Mesh>;

class Renderer
{
public:
    Renderer();
    Renderer(std::shared_ptr<textures::Loader> _texLoader, std::shared_ptr<shaders::Block> _blockShader);

    void renderAll();

    template<typename T>
    errors::expected<RenderId> registerNew(T toRegister);
    template<typename T>
    errors::expected<> registerExisting(RenderId id, T toRegister);
    errors::expected<> unregister(RenderId id);

private:
    template<typename T>
    errors::expected<std::unique_ptr<MeshTypes>> generateMesh(T pre);

    RenderId getNewId();

    std::shared_ptr<textures::Loader> texLoader;

    // Shaders
	std::shared_ptr<shaders::Block> blockShader;

    std::map<RenderId, std::unique_ptr<MeshTypes>> meshes;

    RenderId nextId {1};
};

/********************************
 * Definitions
********************************/

inline
Renderer::Renderer()
{
    texLoader = std::make_shared<textures::Loader>();

    // Shaders
	blockShader = std::make_shared<shaders::Block>(texLoader);
}

inline
Renderer::Renderer(std::shared_ptr<textures::Loader> _texLoader, std::shared_ptr<shaders::Block> _blockShader)
{
    texLoader = _texLoader;
    blockShader = _blockShader;
}

inline
void
Renderer::renderAll()
{
    struct MeshRenderer
    {
        void operator()(Chunk_Mesh& cMesh) const { cMesh.doRender(); }
    };

    // TODO: Add locking
    for (auto meshIter = meshes.begin(); meshIter != meshes.end(); meshIter++)
    {
        //Chunk_Mesh cMesh = std::get<Chunk_Mesh>(*meshIter->second);
        //cMesh.doRender();
        std::visit(MeshRenderer{}, *meshIter->second);
    }
}

template<typename T>
inline
errors::expected<RenderId>
Renderer::registerNew(T toRegister)
{
    errors::expected<std::unique_ptr<MeshTypes>> newMesh = generateMesh(toRegister);
    if (errors::has_error(newMesh))
    {
        return std::unexpected(newMesh.error().prefix("Renderer: registerNew: "));
    }

    RenderId id = getNewId();
    meshes.emplace(id, std::move(newMesh.value()));
    //std::cout << "registerNew Id=" << id << std::endl;
    return id;
}

template<typename T>
inline
errors::expected<>
Renderer::registerExisting(RenderId id, T toRegister)
{
    if (id == 0)
    {
        return errors::unexpected("Renderer: registerExisting: 0 is invalid Id", errors::Code::InvalidArgument);
    }
    if (!meshes.contains(id))
    {
        return errors::unexpected("Renderer: registerExisting: no mesh with Id="+std::to_string(id), errors::Code::NotFound);
    }

    errors::expected<std::unique_ptr<MeshTypes>> newMesh = generateMesh(toRegister);
    if (errors::has_error(newMesh))
    {
        return std::unexpected(newMesh.error().prefix("Renderer: registerExisting: "));
    }

    meshes[id] = std::move(newMesh.value());
    //std::cout << "registerExisting Id=" << id << std::endl;
    return {};
}

inline
errors::expected<>
Renderer::unregister(RenderId id)
{
    size_t numErased = meshes.erase(id);
    if (numErased == 0)
    {
        return errors::unexpected("Renderer: unregister: no mesh with Id="+std::to_string(id), errors::Code::NotFound);
    }

    return {};
}

template<>
inline
errors::expected<std::unique_ptr<MeshTypes>>
Renderer::generateMesh(Chunk* pre)
{
    std::vector<Block_Face> blockFaces;
	Block_Face blockFace;

    textures::TextureSet texSet = texLoader->getTextureSet("blocks");

	for (int x = 0; x < CHUNK_MAX_WIDTH; x++) {
		for (int y = 0; y < CHUNK_MAX_HEIGHT; y++) {
			for (int z = 0; z < CHUNK_MAX_WIDTH; z++) {
				if (pre->getBlock(x, y, z)->getType() == AIR) {
					// Add surrounding faces

					// XPOS
					if (x != 0) {
						if (pre->getBlock(x - 1, y, z)->getType() != AIR) {
							blockFace.Position = glm::vec3(x, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(1, 0, 0);
                            blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(x - 1, y, z)->getType(), XPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// XNEG
					if (x != CHUNK_MAX_WIDTH - 1) {
						if (pre->getBlock(x + 1, y, z)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 1, y + 0.5f, z + 0.5f);
							blockFace.Normal = glm::vec3(-1, 0, 0);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(x + 1, y, z)->getType(), XNEG);
							blockFaces.push_back(blockFace);
						}
					}
					// YPOS
					if (y != 0) {
						if (pre->getBlock(x, y - 1, z)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y, z + 0.5f);
							blockFace.Normal = glm::vec3(0, 1, 0);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(x, y - 1, z)->getType(), YPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// YNEG
					if (y != CHUNK_MAX_HEIGHT - 1) {
						if (pre->getBlock(x, y + 1, z)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 1, z + 0.5f);
							blockFace.Normal = glm::vec3(0, -1, 0);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(x, y + 1, z)->getType(), YNEG);
							blockFaces.push_back(blockFace);
						}
					}
					// ZPOS
					if (z != 0) {
						if (pre->getBlock(x, y, z - 1)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z);
							blockFace.Normal = glm::vec3(0, 0, 1);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(x, y, z - 1)->getType(), ZPOS);
							blockFaces.push_back(blockFace);
						}
					}
					// ZNEG
					if (z != CHUNK_MAX_WIDTH - 1) {
						if (pre->getBlock(x, y, z + 1)->getType() != AIR) {
							blockFace.Position = glm::vec3(x + 0.5f, y + 0.5f, z + 1);
							blockFace.Normal = glm::vec3(0, 0, -1);
							blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(x, y, z + 1)->getType(), ZNEG);
							blockFaces.push_back(blockFace);
						}
					}
				}
			}
		}
	}

    // TODO: Do properly
    // Temp fix for chunk borders: Assume AIR if outside curr chunk
    for (int w = 0; w < CHUNK_MAX_WIDTH; w++)
    {
        for (int y = 0; y < CHUNK_MAX_HEIGHT; y++)
        {
            // XPOS
            if (pre->getBlock(CHUNK_MAX_WIDTH-1, y, w)->getType() != AIR)
            {
                blockFace.Position = glm::vec3(CHUNK_MAX_WIDTH, y + 0.5f, w + 0.5f);
                blockFace.Normal = glm::vec3(1, 0, 0);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(CHUNK_MAX_WIDTH-1, y, w)->getType(), XPOS);
                blockFaces.push_back(blockFace);
            }
            // XNEG
            if (pre->getBlock(0, y, w)->getType() != AIR)
            {
                blockFace.Position = glm::vec3(0, y + 0.5f, w + 0.5f);
                blockFace.Normal = glm::vec3(-1, 0, 0);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(0, y, w)->getType(), XNEG);
                blockFaces.push_back(blockFace);
            }
            // ZPOS
            if (pre->getBlock(w, y, CHUNK_MAX_WIDTH-1)->getType() != AIR)
            {
                blockFace.Position = glm::vec3(w + 0.5f, y + 0.5f, CHUNK_MAX_WIDTH);
                blockFace.Normal = glm::vec3(0, 0, 1);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(w, y, CHUNK_MAX_WIDTH-1)->getType(), ZPOS);
                blockFaces.push_back(blockFace);
            }
            // ZNEG
            if (pre->getBlock(w, y, 0)->getType() != AIR)
            {
                blockFace.Position = glm::vec3(w + 0.5f, y + 0.5f, 0);
                blockFace.Normal = glm::vec3(0, 0, -1);
                blockFace.TexCoords = texSet.getTileCoords(pre->getBlock(w, y, 0)->getType(), ZNEG);
                blockFaces.push_back(blockFace);
            }
        }
    }

    std::unique_ptr<MeshTypes> newMesh = std::make_unique<MeshTypes>(Chunk_Mesh(blockFaces, glGetUniformLocation(blockShader->getProgram(), "model"),
        glm::translate(glm::mat4(1), glm::vec3(pre->getXPos(), 0, pre->getZPos())), false));

    struct MeshVisitor
    {
        std::shared_ptr<shaders::Block> blockShader;

        void operator()(Chunk_Mesh& cMesh) const { cMesh.setShader(blockShader); }
    };

    MeshVisitor mv;
    mv.blockShader = blockShader;
    std::visit(mv, *newMesh);

    return newMesh;
}

inline
RenderId
Renderer::getNewId()
{
    // TODO: Add locking if needed
    // TODO: Ensure no existing Ids can be generated
    return nextId++;
}

} // namespace renderer