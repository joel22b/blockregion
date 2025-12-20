#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shaders/block.h>
#include <errors/br-expected.h>

#include <spdlog/spdlog.h>

using namespace std;

struct Block_Face
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
    glm::vec2 TexCoords;
};

class Chunk_Mesh
{
public:
    /*  Functions  */
    Chunk_Mesh() {
        this->readyToRender = false;
    }

    // Constructor
    Chunk_Mesh(vector<Block_Face> vertices, bool ready = true) {
        this->readyToRender = false;

        this->vertices = vertices;

        if (ready)
        {
            // Now that we have all the required data, set the vertex buffers and its attribute pointers.
            if (errors::expected<> ret = setupMesh(); errors::has_error(ret))
            {
                spdlog::get("blockregion")->error("{}", ret.error());
                return;
            }
            this->readyToRender = true;
        }
    }

    Chunk_Mesh(vector<Block_Face> vertices, GLuint _modelLoc,
        //glm::f32 _modelInfo,
        glm::mat<4, 4, float, glm::packed_highp> _modelInfo,
        bool ready = true) {
        this->readyToRender = false;

        this->vertices = vertices;

        modelLoc = _modelLoc;
        modelInfo = _modelInfo;

        if (ready)
        {
            // Now that we have all the required data, set the vertex buffers and its attribute pointers.
            if (errors::expected<> ret = setupMesh(); errors::has_error(ret))
            {
                spdlog::get("blockregion")->error("{}", ret.error());
                return;
            }
            this->readyToRender = true;
        }
    }

    void setShader(std::shared_ptr<shaders::Block> _shader)
    {
        shader = _shader;
    }

    errors::expected<>
    doRender()
    {
        if (readyToRender) {
            if (errors::expected<> ret = Draw(); errors::has_error(ret))
            {
                return ret;
            }
        }
        else
        {
            if (errors::expected<> ret = setupMesh(); errors::has_error(ret))
            {
                return ret;
            }
            readyToRender = true;
            if (errors::expected<> ret = Draw(); errors::has_error(ret))
            {
                return ret;
            }
        }

        return {};
    }

    bool ready() {
        return readyToRender;
    }

private:
    /*  Mesh Data  */
    vector<Block_Face> vertices;
    std::shared_ptr<shaders::Block> shader;

    GLuint modelLoc;
    //glm::f32 modelInfo;
    glm::mat<4, 4, float, glm::packed_highp> modelInfo;

    /*  Render data  */
    GLuint VAO, VBO;
    bool readyToRender = false;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    errors::expected<> setupMesh()
    {
        //std::cout << "Setup Mesh" << std::endl;
        // Create buffers/arrays
        glGenVertexArrays(1, &VAO);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Gen VAO failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        glGenBuffers(1, &VBO);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Gen VBO failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }

        glBindVertexArray(this->VAO);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Bind VAO failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Bind VBO failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Block_Face), &this->vertices[0], GL_STATIC_DRAW);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Load data failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Enable vertex attrib 0 failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)0);
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Set vertex attrib 0 failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        // Vertex Normals
        glEnableVertexAttribArray(1);
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Enable vertex attrib 1 failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)offsetof(Block_Face, Normal));
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Set vertex attrib 1 failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Enable vertex attrib 2 failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)offsetof(Block_Face, TexCoords));
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: setupMesh: Set vertex attrib 2 failed: 0x{:x}", err), errors::Code::InvalidArgument);
        }

        glBindVertexArray(0);

        return {};
    }

    // Render the mesh
    errors::expected<> Draw()
    {
        if (!shader)
        {
            return errors::unexpected("Tried to draw chunk without shader", errors::Code::InvalidState);
        }

        shader->Use();
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelInfo));
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            return errors::unexpected(fmt::format("Chunk_Mesh: Draw: model update failed: 0x{:x}", err), errors::Code::Unknown);
        }

        if (errors::expected<> ret = shader->bindTextures(); errors::has_error(ret))
        {
            return ret;
        }
        if (errors::expected<> ret = shader->draw(VAO, vertices.size()); errors::has_error(ret))
        {
            return ret;
        }
        if (errors::expected<> ret = shader->unbindTextures(); errors::has_error(ret))
        {
            return ret;
        }

        return {};
    }
};
