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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "camera.h"
#include "game.h"
#include "text.h"

//#include "src/utils/Logger.h"
//#define LOG(severity, msg) Logger::log("main.cpp", severity, msg)

// Window dimensions
const GLint WIDTH = 1600, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int nbFrames = 0;
double currentTime;
double lastTime = glfwGetTime();
std::string mspfText = "test", fpsText = "test";

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//Logger* logger;

Game* game;

int main() {
    //logger = new Logger();

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
        std::cout << "Error main window: " << err << std::endl;
    }

    // Creates the window (The two nullptrs are monitor and window respectively)
    //std::ostringstream msg;
	//msg << "Creating GFLW window: width=" << WIDTH << " height=" << HEIGHT;
	//LOG(INFO, msg.str());
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Blockzone", nullptr, nullptr);

    // Adjusts for pixel density
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    if (window == nullptr) {
        //LOG(ERROR, "Failed to create GLFW window");
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    //LOG(INFO, "Setting callbacks for keyboard and mouse");
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error main callback: " << err << std::endl;
    }

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        //LOG(ERROR, "Failed to initialise GLEW");

        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error main viewport: " << err << std::endl;
    }

    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cout << "Error main blend: " << err << std::endl;
    }

    game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);

    std::string textPath{TEXTURES_PATH};
    textPath += "/arial.ttf";
    Text arialText = Text(textPath.c_str(), 20);

    // Main program loop
    //LOG(INFO, "Entering main loop");
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
        }

        glfwPollEvents();
        game->doInput(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        game->doUpdate(deltaTime);

        game->doRender(&arialText);

        arialText.RenderText(mspfText, 25.0f, 50.0f, 1.0f, glm::vec3(1, 1, 1));
        arialText.RenderText(fpsText, 25.0f, 25.0f, 1.0f, glm::vec3(1, 1, 1));
        // Drawing done above

        glfwSwapBuffers(window);
    }
    //LOG(INFO, "Exiting main loop");

    // If here, program is exiting
    delete game;

    //LOG(INFO, "Terminating GLFW");

    glfwTerminate();
    
	//delete logger;

    std::cout << "Done" << std::endl;

    return EXIT_SUCCESS;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    game->keyCallback(window, key, scancode, action, mode);
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    game->mouseCallback(window, xPos, yPos);
}