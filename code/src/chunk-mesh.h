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
//#include "texture-loader.h"

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
    //vector<Texture> textures;
    std::shared_ptr<shaders::Block> shader;

    /*  Render data  */
    GLuint VAO, VBO;
    bool readyToRender = false;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh()
    {
        // Create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(this->VAO);
        // Load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Block_Face), &this->vertices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)0);
        // Vertex Normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)offsetof(Block_Face, Normal));
        // Vertex Texture Coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Block_Face), (GLvoid*)offsetof(Block_Face, TexCoords));

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

        shader->bindTextures();
        shader->draw(VAO, vertices.size());
        shader->unbindTextures();
    }
};
