#pragma once

#include <string>

#include <renderer/types.h>

#include <spdlog/spdlog.h>

namespace text
{

class Text
{
public:
    Text():
        m_logger(spdlog::get("blockregion"))
    {}
    
    errors::expected<> initialize(std::string text, int x, int y, float scale, glm::vec3 color)
        // text(text), x(x), y(y), scale(scale), color(color)
    {
        this->text = text;
        this->x = x;
        this->y = y;
        this->scale = scale;
        this->color = color;

        errors::expected<renderer::RenderId> registerId = renderer::getGlobalRenderer()->registerNew(this);
        if (errors::has_error(registerId))
        {
            m_logger->error("Failed to register text for rendering: {}", registerId.error());
            return errors::unexpected(registerId.error());
        }

        renderId = registerId.value();

        return {};
    }

    ~Text()
    {
        errors::expected<> unregisterRet = renderer::getGlobalRenderer()->unregister(renderId);
        if (errors::has_error(unregisterRet))
        {
            m_logger->warn("Failed to unregister text: {}", unregisterRet.error());
        }
    }

    errors::expected<> updateText(std::string newText)
    {
        text = newText;
        errors::expected<> regRet = renderer::getGlobalRenderer()->registerExisting(renderId, this);
        return regRet;
    }

//private:
    std::string text;
    int x;
    int y;
    float scale;
    glm::vec3 color;

    renderer::RenderId renderId;

    std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace text
