#pragma once

#include <cstdint>
#include <variant>

// List all meshes
#include <meshes/chunk-mesh.h>
#include <meshes/text.h>

namespace renderer
{

using RenderId = uint32_t;
using MeshTypes = std::variant<Chunk_Mesh, meshes::Text>;

//class Renderer;

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
    errors::expected<std::shared_ptr<MeshTypes>> generateMesh(T pre);

    RenderId getNewId();

    std::shared_ptr<textures::Loader> texLoader;

    // Shaders
	std::shared_ptr<shaders::Block> blockShader;
    std::shared_ptr<shaders::Text> textShader;

    std::map<RenderId, std::shared_ptr<MeshTypes>> meshes;

    RenderId nextId {1};
};

} // namespace renderer
