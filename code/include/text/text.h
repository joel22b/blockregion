#pragma once

#include <string>

#include <renderer/types.h>

namespace text
{

class Text
{
public:
    Text() {}
    
    errors::expected<> initialize(std::shared_ptr<renderer::Renderer> renderer, std::string text, int x, int y, float scale, glm::vec3 color)
        //renderer(renderer), text(text), x(x), y(y), scale(scale), color(color)
    {
        this->renderer = renderer;
        this->text = text;
        this->x = x;
        this->y = y;
        this->scale = scale;
        this->color = color;

        errors::expected<renderer::RenderId> registerId = renderer->registerNew(this);
        if (errors::has_error(registerId))
        {
            std::cout << "Failed to register text for rendering: " << registerId.error() << std::endl;
            return errors::unexpected(registerId.error());
        }

        renderId = registerId.value();

        return {};
    }

    ~Text()
    {
        errors::expected<> unregisterRet = renderer->unregister(renderId);
        if (errors::has_error(unregisterRet))
        {
            std::cout << "Failed to unregister text: " << unregisterRet.error() << std::endl;
        }
    }

    errors::expected<> updateText(std::string newText)
    {
        text = newText;
        errors::expected<> regRet = renderer->registerExisting(renderId, this);
        return regRet;
    }

//private:
    std::string text;
    int x;
    int y;
    float scale;
    glm::vec3 color;

    std::shared_ptr<renderer::Renderer> renderer;
    renderer::RenderId renderId;
};

} // namespace text
