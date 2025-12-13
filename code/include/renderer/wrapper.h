#pragma once

#include <concepts>

#include <spdlog/spdlog.h>

#include <renderer/renderer.h>

namespace renderer
{

template <typename C>
concept HasUpdateFunction = requires(C obj)
{
    { obj.update() } -> std::same_as<void>;
};

template <typename T, bool AutoRegister = true>
struct Wrapper
{
public:
    Wrapper(std::shared_ptr<Renderer> renderer, std::shared_ptr<T> object);
    ~Wrapper();

    std::shared_ptr<T>
    operator->() const
    {
        return object;
    }

    errors::expected<> update();

private:
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<T> object;
    RenderId renderId {INVALID_ID};
};

/********************************
 * Definitions
********************************/

template <typename T, bool AutoRegister>
Wrapper<T, AutoRegister>::Wrapper(std::shared_ptr<Renderer> renderer, std::shared_ptr<T> object) :
    renderer(renderer), object(object)
{
    if (AutoRegister)
    {
        errors::expected<RenderId> retId = renderer->registerNew(object);
        if (errors::has_error(retId))
        {
            //spdlog::get("blockregion")->error("Renderer Wrapper construct failed to auto register object: {}", retId.error());
            return;
        }
        renderId = retId.value();
    }
}

template <typename T, bool AutoRegister>
Wrapper<T, AutoRegister>::~Wrapper()
{
    renderer->unregister(renderId);
}

template <typename T, bool AutoRegister>
errors::expected<>
Wrapper<T, AutoRegister>::update()
{
    if constexpr (HasUpdateFunction<T>)
    {
        object.update();
    }

    errors::expected<> ret = renderer->registerExisting(renderId, object);
    if (errors::has_error(ret))
    {
        return ret;
    }

    return {};
}

} // namespace renderer
