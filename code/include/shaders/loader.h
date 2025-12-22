#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <optional>
#include <filesystem>

#include <GL/glew.h>

#include <errors/br-expected.h>
#include <utils/get-folder.h>

#include <spdlog/fmt/fmt.h>

namespace shaders
{

class Loader
{
public:
    errors::expected<GLuint> load(std::string filename, GLenum type);

private:
    errors::expected<std::string> loadFile(std::string path);
    errors::expected<GLuint> compileShader(std::string code, GLenum type);
};

/********************************
 * Definitions
********************************/

inline
errors::expected<GLuint>
Loader::load(std::string filename, GLenum type)
{
    std::filesystem::path shaderPath = utils::get_executable_dir().append(SHADERS_PATH).append(filename);
    
    errors::expected<std::string> code = loadFile(shaderPath.native());
    if (errors::has_error(code))
    {
        return errors::unexpected(code.error());
    }

    return compileShader(code.value(), type);
}

inline
errors::expected<std::string>
Loader::loadFile(std::string path)
{
    std::ifstream file(path);

    if (file.is_open())
    {
        std::string file_contents((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        return file_contents;
        file.close();
    }
    else
    {
        return errors::unexpected(fmt::format("Shader not found from disk: [{}]", path), errors::Code::NotFound);
    }
    return {};
}

inline
errors::expected<GLuint>
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
        std::string errMsg {"Shader compilation failed for type="};
        errMsg += std::to_string(type);
        errMsg += " Error:";
        errMsg += "\n\r";
        errMsg += infoLog;
        return errors::unexpected(errMsg, errors::Code::InitializationError);
    }
    return glShader;
}

} // namespace shaders