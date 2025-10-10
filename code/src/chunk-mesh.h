#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaders.h"
#include "texture-loader.h"

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
    Chunk_Mesh(vector<Block_Face> vertices, vector<Texture> textures) {
        this->readyToRender = false;

        this->vertices = vertices;
        this->textures = textures;

        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh();
        this->readyToRender = true;
    }

    Chunk_Mesh(vector<Block_Face> vertices) {
        this->readyToRender = false;

        this->vertices = vertices;
    }

    void doRender(Shader shader) {
        if (readyToRender) {
            Draw(shader);
        }
    }

    bool ready() {
        return readyToRender;
    }

    void doSetup(vector<Texture> textures) {
        this->textures = textures;

        this->setupMesh();
        this->readyToRender = true;
    }

private:
    /*  Mesh Data  */
    vector<Block_Face> vertices;
    vector<Texture> textures;

    /*  Render data  */
    GLuint VAO, VBO;
    bool readyToRender = false;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh()
    {
        // Create buffers/arrays
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);

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
    void Draw(Shader shader)
    {
        shader.Use();

        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;

        for (GLuint i = 0; i < this->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
            // Retrieve texture number (the N in diffuse_textureN)
            stringstream ss;
            string number;
            string name = this->textures[i].type;

            if (name == "texture_diffuse")
            {
                ss << diffuseNr++; // Transfer GLuint to stream
            }
            else if (name == "texture_specular")
            {
                ss << specularNr++; // Transfer GLuint to stream
            }

            number = ss.str();
            // Now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.getProgram(), (name + number).c_str()), i);
            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }

        // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
        glUniform1f(glGetUniformLocation(shader.getProgram(), "material.shininess"), 32.0f);

        // Draw mesh
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_POINTS, 0, vertices.size());
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        for (GLuint i = 0; i < this->textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
};
