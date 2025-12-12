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

constexpr RenderId INVALID_ID = 0;

// Forward Declaration
class Renderer;

} // namespace renderer
