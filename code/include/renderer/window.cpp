#include <renderer/window.h>

#include <string>

namespace renderer
{

// Primary monitor fullscreen
Window::Window():
    m_logger(spdlog::get("blockregion"))
{
    // By default get primary monitor
    GLFWmonitor* primary = glfwGetPrimaryMonitor();

    // Get the current video mode of that monitor (to match its resolution/Hz)
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    m_logger->debug("Primary monitor settings: width={} height={} refresh_rate={}", mode->width, mode->height, mode->refreshRate);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    // Creates the window (The two nullptrs are monitor and window respectively)
    std::string windowName {"blockregion"};
    window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), primary, nullptr);
    if (window == NULL)
    {
        m_logger->error("Failed to create fullscreen [{}] with width={} height={}", windowName, mode->width, mode->height);
        return;
    }

    initializeWindow();

    glfwSetWindowMonitor(window, primary, 0, 0, mode->width, mode->height, mode->refreshRate);

    m_logger->info("Created fullscreen [{}]: width={} height={}", windowName, width, height);
}

// Windowed with specific dimensions
Window::Window(int desiredWidth, int desiredHeight):
    m_logger(spdlog::get("blockregion"))
{
    // Creates the window (The two nullptrs are monitor and window respectively)
    std::string windowName {"blockregion"};
    window = glfwCreateWindow(desiredWidth, desiredHeight, windowName.c_str(), nullptr, nullptr);
    if (window == NULL)
    {
        m_logger->error("Failed to create window [{}] with width={} height={}", windowName, desiredWidth, desiredHeight);
        return;
    }

    initializeWindow();

    m_logger->info("Created window [{}]: width={} height={}", windowName, width, height);
}

void
Window::initializeWindow()
{
    // Adjusts for pixel density
    glfwGetFramebufferSize(window, &width, &height);

    glfwMakeContextCurrent(window);

    setVSync(true);

    glewExperimental = GL_TRUE;

    if (GLenum err = glewInit(); err != GLEW_OK)
    {
        m_logger->error("Renderer createWindow glew init failed with error: 0x{:x}", err);
        return;
    }

    glViewport(0, 0, width, height);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        m_logger->error("Renderer createWindow glew viewport failed with width={} height={} with error {:x}", width, height, err);
        return;
    }

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        m_logger->error("Renderer createWindow glew setup blend failed with error {:x}", err);
        return;
    }
}

Window::~Window()
{
    glfwDestroyWindow(window);
}

void
Window::requestClose()
{
    m_logger->debug("Window close requested");

    if (window == nullptr)
    {
        // Doesn't exist so no action needed
        return;
    }

    glfwSetWindowShouldClose(window, GL_TRUE);
}

bool
Window::hasWindow()
{
    return glfwWindowShouldClose(window);
}

void
Window::swapBuffers()
{
    // TODO: handle possiblity of window being null
    glfwSwapBuffers(window);
}

float
Window::getAspectRatio()
{
    return static_cast<float>(width)/static_cast<float>(height);
}

void
Window::setVSync(bool enabled)
{
    glfwMakeContextCurrent(window);

    // Enable or disable VSync
    glfwSwapInterval(enabled); // 0 = off, 1 = on
}

void
Window::registerKeyboardCallback(KeyboardCallback newKeyboardCallback)
{
    if (window == nullptr)
    {
        m_logger->warn("Cannot register keyboard callback with no window setup");
        return;
    }

    this->m_keyboardCallback = newKeyboardCallback; 
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // Retrieve the 'this' pointer
        auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (instance && instance->m_keyboardCallback) {
            instance->m_keyboardCallback(key, scancode, action, mods);
        }
    });
}

void
Window::registerMouseCallback(MouseCallback newMouseCallback)
{
    if (window == nullptr)
    {
        m_logger->warn("Cannot register mouse callback with no window setup");
        return;
    }

    this->m_mouseCallback = newMouseCallback; 
    glfwSetWindowUserPointer(window, this);

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos)
    {
        // Retrieve the 'this' pointer
        auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (instance && instance->m_mouseCallback) {
            instance->m_mouseCallback(xPos, yPos);
        }
    });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

} // namespace renderer
