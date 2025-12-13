#include <iostream>
#include <string>

// GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "SOIL2/SOIL2.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "game.h"

// Window dimensions
const GLint WIDTH = 1600, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int nbFrames = 0;
double currentTime;
double lastTime = glfwGetTime();
std::string mspfText = "test", fpsText = "test";

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Game* game;

int main() {
    // Create logger first
    std::shared_ptr<spdlog::logger> logger;
    {
        std::vector<spdlog::sink_ptr> sinks;
        spdlog::sink_ptr sinkConsole = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        spdlog::sink_ptr sinkFile = std::make_shared<spdlog::sinks::basic_file_sink_mt>("blockregion.log", true);

        sinkConsole->set_level(spdlog::level::debug);
        sinkFile->set_level(spdlog::level::trace);

        sinkConsole->set_pattern("[%T.%f] [%^%L%$] %n: %v");
        sinkFile->set_pattern("[%T.%f] [%^%L%$] %n: %v");

        sinks.push_back(sinkConsole);
        sinks.push_back(sinkFile);

        logger = std::make_shared<spdlog::logger>("blockregion", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(logger);
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_every(std::chrono::seconds(1));

        logger->debug("Logger created successfully");
    }
    logger->info("Version: {}", VERSION_STRING);

    glfwInit();

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        logger->critical("Failed to setup glfw window hints: {}", err);
    }

    // Creates the window (The two nullptrs are monitor and window respectively)
    //std::ostringstream msg;
	//msg << "Creating GFLW window: width=" << WIDTH << " height=" << HEIGHT;
	//LOG(INFO, msg.str());
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "blockregion", nullptr, nullptr);
    if (window == NULL)
    {
        logger->critical("Failed to create glfw window [{}] with width={} height={}", "blockregion", WIDTH, HEIGHT);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    logger->info("Created window: width={} height={}", WIDTH, HEIGHT);

    // Adjusts for pixel density
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        logger->critical("Failed to setup glfw callbacks: {}", err);
        return EXIT_FAILURE;
    }

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        logger->critical("Failed to initialize glew");
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        logger->critical("Failed to setup viewport: {}", err);
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        logger->critical("Failed to setup blend function: {}", err);
        return EXIT_FAILURE;
    }

    game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Main program loop
    logger->debug("Started main loop");
    while (!glfwWindowShouldClose(window)) {
        // Get time since last frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get framerate and ms per frame
        currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= (double)1.0) {
            double mspf = 1000.0 / double(nbFrames);
            mspfText = "ms per frame: " + to_string(mspf);
            fpsText = "fps: " + to_string(1000.0 / mspf);
            nbFrames = 0;
            lastTime += 1.0;

            game->updateFPS(fpsText, mspfText);
        }

        glfwPollEvents();
        game->doInput(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game->doUpdate(deltaTime);

        game->doRender();

        glfwSwapBuffers(window);
    }
    logger->debug("Finished main loop");

    // If here, program is exiting
    delete game;

    glfwTerminate();

    logger->info("Done");

    return EXIT_SUCCESS;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    game->keyCallback(window, key, scancode, action, mode);
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    game->mouseCallback(window, xPos, yPos);
}