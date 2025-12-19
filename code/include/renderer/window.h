#pragma once

#include <memory>
#include <functional>

#include <spdlog/spdlog.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace renderer
{

using KeyboardCallback = std::function<void(int, int, int, int)>;
using MouseCallback = std::function<void(double, double)>;

class Window
{
public:
    Window(); // Primary monitor fullscreen
    Window(int desiredWidth, int desiredHeight); // Windowed
    ~Window();

    void requestClose();
    bool hasWindow();
    void swapBuffers();
    float getAspectRatio();
    void setVSync(bool enabled);

    // Input Handling
    void registerKeyboardCallback(KeyboardCallback newKeyboardCallback);
    void registerMouseCallback(MouseCallback newMouseCallback);

private:
    void initializeWindow();

    std::shared_ptr<spdlog::logger> m_logger;

    GLFWwindow* window;
    int width;
    int height;

    KeyboardCallback m_keyboardCallback;
    MouseCallback m_mouseCallback;
};

} // namespace renderer
