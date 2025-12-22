#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <optional>
#include <vector>
#include <memory>
#include <atomic>

#include <GL/glew.h>

#include <errors/br-expected.h>

#include <textures/loader.h>

namespace shaders
{

template <typename Derived>
class Shader
{
public:
    Shader(std::shared_ptr<textures::Loader> texLoader);

    void Use()
    {
        glUseProgram(program);
    }

    GLuint getProgram()
    {
        return program;
    }

protected:
    errors::expected<> linkProgram() const;

    GLuint program;
    std::atomic<bool> valid {false};

    std::shared_ptr<std::vector<textures::Texture>> texs;

    std::shared_ptr<spdlog::logger> m_logger;
};

/********************************
 * Definitions
********************************/

template <typename Derived>
inline
Shader<Derived>::Shader(std::shared_ptr<textures::Loader> texLoader):
    m_logger(spdlog::get("blockregion"))
{
    program = glCreateProgram();
    if (GLenum err = glGetError(); err != GL_NO_ERROR)
    {
        std::string derivedName {static_cast<const Derived*>(this)->name};
        m_logger->critical("Failed to create shader program for [{}]: {}", derivedName, err);
        return;
    }

    errors::expected<> retAttach = static_cast<const Derived*>(this)->attachShaders();
    if (errors::has_error(retAttach))
    {
        std::string derivedName {static_cast<const Derived*>(this)->name};
        m_logger->critical("Failed to attach shaders for [{}]: {}", derivedName, retAttach.error());
        return;
    }

    errors::expected<> retTextures = static_cast<Derived*>(this)->loadTextures(texLoader);
    if (errors::has_error(retTextures))
    {
        std::string derivedName {static_cast<const Derived*>(this)->name};
        m_logger->critical("Failed to load textures for [{}]: {}", derivedName, retTextures.error());
        return;
    }

    // Success
    valid = true;
}

template <typename Derived>
inline
errors::expected<>
Shader<Derived>::linkProgram() const
{
    if (valid)
    {
        return errors::unexpected(fmt::format("Shader [{}] is in a valid state in call to linkProgram", static_cast<const Derived*>(this)->name), errors::Code::InvalidState);
    }

    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::string errMsg {"ERROR: Shader failed to link program: "};
        errMsg += static_cast<const Derived*>(this)->name;
        errMsg += "\n\r";
        errMsg += infoLog;
        return errors::unexpected(errMsg, errors::Code::InitializationError);
    }

    return {};
}

} // namespace shaders
