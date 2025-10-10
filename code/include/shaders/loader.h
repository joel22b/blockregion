#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <optional>

#include <GL/glew.h>

namespace shaders
{

//constexpr std::string_view filePrefix {"/home/jbraun/projects/blockregion/code/shaders/"};

class Loader
{
public:
    std::optional<GLuint> load(std::string filename, GLenum type);

private:
    std::optional<std::string> loadFile(std::string path);
    std::optional<GLuint> compileShader(std::string code, GLenum type);
};

/********************************
 * Definitions
********************************/

inline
std::optional<GLuint>
Loader::load(std::string filename, GLenum type)
{
    std::string path = std::string(SHADERS_PATH) + filename;
    
    std::optional<std::string> code = loadFile(path);
    if (!code.has_value())
    {
        std::cout << "Failed to load shader file: " << filename << std::endl;
        return std::nullopt;
    }

    return compileShader(code.value(), type);
}

inline
std::optional<std::string>
Loader::loadFile(std::string path)
{
    std::ifstream file(path);

    if (file.is_open())
    {
        std::string file_contents((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        return file_contents;
        file.close();
    } else {
        std::cerr << "Error: Could not open file: " << path << std::endl;
    }
    return std::nullopt;
}

inline
std::optional<GLuint>
Loader::compileShader(std::string code, GLenum type)
{
    const GLchar* glShaderCode = code.c_str();

    GLuint glShader = glCreateShader(type);
    glShaderSource(glShader, 1, &glShaderCode, NULL);
    glCompileShader(glShader);
    // Print compile errors if any
    GLint success;
    glGetShaderiv(glShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(glShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << type << "::COMPILATION_ERROR\n" << infoLog << std::endl;
        return std::nullopt;
    }
    return glShader;
}

} // namespace shaders