#include <variant>
#include <memory>
#include <map>

#include <errors/br-expected.h>

#include <textures/loader.h>

// List all shaders
#include <shaders/block.h>
#include <shaders/text.h>

// List all meshes
#include <meshes/chunk-mesh.h>
#include <meshes/text.h>

#include <renderer/renderer.h>

namespace renderer
{

/********************************
 * Definitions
********************************/
Renderer::Renderer()
{
    texLoader = std::make_shared<textures::Loader>();

    // Shaders
	blockShader = std::make_shared<shaders::Block>(texLoader);
    textShader = std::make_shared<shaders::Text>(texLoader);
}

Renderer::Renderer(std::shared_ptr<textures::Loader> _texLoader, std::shared_ptr<shaders::Block> _blockShader)
{
    texLoader = _texLoader;
    blockShader = _blockShader;
    textShader = std::make_shared<shaders::Text>(texLoader);
}

void
Renderer::renderAll()
{
    struct MeshRenderer
    {
        void operator()(Chunk_Mesh& cMesh) const { cMesh.doRender(); }
        void operator()(meshes::Text& mesh) const { mesh.render(); }
    };

    // TODO: Add locking
    for (auto meshIter = meshes.begin(); meshIter != meshes.end(); meshIter++)
    {
        //Chunk_Mesh cMesh = std::get<Chunk_Mesh>(*meshIter->second);
        //cMesh.doRender();
        std::visit(MeshRenderer{}, *meshIter->second);
    }
}

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

RenderId
Renderer::getNewId()
{
    // TODO: Add locking if needed
    // TODO: Ensure no existing Ids can be generated
    return nextId++;
}

} // namespace renderer