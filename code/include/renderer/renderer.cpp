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

// OpenGL includes
#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace renderer
{

/********************************
 * Definitions
********************************/
Renderer::Renderer()
{
    m_logger = spdlog::get("blockregion");

    //window = std::make_shared<Window>(1600, 800);
    window = std::make_shared<Window>();

    texLoader = std::make_shared<textures::Loader>();

    // Shaders
	blockShader = std::make_shared<shaders::Block>(texLoader);
    textShader = std::make_shared<shaders::Text>(texLoader);
}

Renderer::~Renderer()
{
    glfwTerminate();
}

void
Renderer::updateFOV(float fov)
{
    blockShader->Use();
	glm::mat4 projection(1);
	projection = glm::perspective(fov, window->getAspectRatio(), 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(blockShader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void
Renderer::updateCamera(glm::mat4 viewMatrix, world::Coord position)
{
    blockShader->Use();
	glUniformMatrix4fv(glGetUniformLocation(blockShader->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(glGetUniformLocation(blockShader->getProgram(), "viewPos"), position.x, position.y, position.z);
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

    window->swapBuffers();
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