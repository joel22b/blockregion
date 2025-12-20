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

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int nbFrames = 0;
double currentTime;
double lastTime = glfwGetTime();
std::string mspfText = "test", fpsText = "test";

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

    // Setup OpenGL
    {
        glfwInit();

        // Set OpenGL version to 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    }

    std::unique_ptr<Game> game = std::make_unique<Game>();

    // Main program loop
    logger->debug("Started main loop");
    while (!renderer::getGlobalRenderer()->getWindow()->hasWindow()) {
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

        game->doUpdate(deltaTime);

        game->doRender();

        renderer::getGlobalRenderer()->renderAll();
    }
    logger->debug("Finished main loop");

    // If here, program is exiting
    game = nullptr;

    // Cleanup OpenGL
    {
        glfwTerminate();
    }

    logger->info("Done");

    return EXIT_SUCCESS;
}
