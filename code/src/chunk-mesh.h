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

#include "shaders/block.h"

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
            this->setupMesh();
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
            this->setupMesh();
            this->readyToRender = true;
        }
    }

    void setShader(std::shared_ptr<shaders::Block> _shader)
    {
        shader = _shader;
    }

    void doRender() {
        if (readyToRender) {
            Draw();
        }
        else
        {
            setupMesh();
            readyToRender = true;
            Draw();
        }
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
    void setupMesh()
    {
        //std::cout << "Setup Mesh" << std::endl;
        // Create buffers/arrays
        glGenVertexArrays(1, &VAO);
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Gen VAO failed: " << err << std::endl;
        }
        glGenBuffers(1, &VBO);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Gen VBO failed: " << err << std::endl;
        }

        glBindVertexArray(this->VAO);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Bind VAO failed: " << err << std::endl;
        }
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Bind VBO failed: " << err << std::endl;
        }
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Block_Face), &this->vertices[0], GL_STATIC_DRAW);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Load data failed: " << err << std::endl;
        }

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Enable vertex attrib 0 failed: " << err << std::endl;
        }
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)0);
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Set vertex attrib 0 failed: " << err << std::endl;
        }
        // Vertex Normals
        glEnableVertexAttribArray(1);
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Enable vertex attrib 1 failed: " << err << std::endl;
        }
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)offsetof(Block_Face, Normal));
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Set vertex attrib 1 failed: " << err << std::endl;
        }
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Enable vertex attrib 2 failed: " << err << std::endl;
        }
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)offsetof(Block_Face, TexCoords));
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: setupMesh: Set vertex attrib 2 failed: " << err << std::endl;
        }

        glBindVertexArray(0);
    }

    // Render the mesh
    void Draw()
    {
        if (!shader)
        {
            std::cout << "TRIED TO DRAW CHUNK WITHOUT SHADER" << std::endl;
            return;
        }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelInfo));
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cout << "Chunk_Mesh: Draw: model update failed: " << err << std::endl;
        }

        shader->bindTextures();
        shader->draw(VAO, vertices.size());
        shader->unbindTextures();
    }
};
