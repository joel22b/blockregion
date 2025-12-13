#pragma once

#include <cstdint>
#include <optional>

#include <spdlog/fmt/fmt.h>

#include "glm/glm.hpp"

namespace world
{

namespace consts
{
const int CHUNK_MAX_WIDTH = 16;
const int CHUNK_MAX_HEIGHT = 64;
} // namespace consts

struct Coord;
struct GridCoord;
struct ChunkCoord;

struct Coord
{
    float x;
    float y;
    float z;

    Coord(float x, float y, float z):
        x(x), y(y), z(z) {}
    Coord(int32_t x, int32_t y, int32_t z):
        x(static_cast<float>(x)), y(static_cast<float>(y)), z(static_cast<float>(z)) {}

    glm::vec3 getVec() const
    {
        return glm::vec3(x, y, z);
    }

    Coord& operator=(const glm::vec3 vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }
};

struct GridCoord
{
    int32_t x;
    int32_t y;
    int32_t z;

    GridCoord(int32_t x, int32_t y, int32_t z):
        x(x), y(y), z(z) {}
    GridCoord(ChunkCoord coord);

    glm::vec3 getVec() const
    {
        return glm::vec3(x, y, z);
    }

    GridCoord& operator=(const glm::vec3 vec)
    {
        x = static_cast<int32_t>(vec.x);
        y = static_cast<int32_t>(vec.y);
        z = static_cast<int32_t>(vec.z);
        return *this;
    }

    GridCoord& operator=(const ChunkCoord coord);
};

struct ChunkCoord
{
    int32_t x;
    int32_t z;

    ChunkCoord(int32_t x, int32_t z):
        x(x), z(z) {}
    ChunkCoord(Coord coord):
        x(coord.x/consts::CHUNK_MAX_WIDTH), z(coord.z/consts::CHUNK_MAX_WIDTH) {}
    ChunkCoord(GridCoord coord):
        x(coord.x/consts::CHUNK_MAX_WIDTH), z(coord.z/consts::CHUNK_MAX_WIDTH) {}

    glm::vec3 getVec()
    {
        return glm::vec3(x, 0.0f, z);
    }

    ChunkCoord& operator=(const Coord& coord)
    {
        x = coord.x/consts::CHUNK_MAX_WIDTH;
        z = coord.z/consts::CHUNK_MAX_WIDTH;
        return *this;
    }

    ChunkCoord& operator=(const GridCoord& coord)
    {
        x = coord.x/consts::CHUNK_MAX_WIDTH;
        z = coord.z/consts::CHUNK_MAX_WIDTH;
        return *this;
    }

    bool operator==(const ChunkCoord& other)
    {
        return (x == other.x) && (z == other.z);
    }
};

inline
GridCoord::GridCoord(ChunkCoord coord):
    x(coord.x * consts::CHUNK_MAX_WIDTH), y(0), z(coord.z * consts::CHUNK_MAX_WIDTH) {}

inline
GridCoord&
GridCoord::operator=(const ChunkCoord coord)
{
    x = coord.x * consts::CHUNK_MAX_WIDTH;
    y = 0;
    z = coord.z * consts::CHUNK_MAX_WIDTH;
    return *this;
}

} // namespace world

namespace fmt
{

template <>
struct formatter<world::Coord>
{
    constexpr auto parse(format_parse_context& ctx)
    { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const world::Coord& coord, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "Coord({}, {}, {})", coord.x, coord.y, coord.z);
    }
};

template <>
struct formatter<world::GridCoord>
{
    constexpr auto parse(format_parse_context& ctx)
    { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const world::GridCoord& coord, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "Grid({}, {}, {})", coord.x, coord.y, coord.z);
    }
};

template <>
struct formatter<world::ChunkCoord>
{
    constexpr auto parse(format_parse_context& ctx)
    { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const world::ChunkCoord& coord, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "Chunk({}, {})", coord.x, coord.z);
    }
};

} // namespace fmt
