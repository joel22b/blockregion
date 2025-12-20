#include "game.h"

Game::Game()
{
	renderer::getGlobalRenderer()->updateFOV(45.0f);
	renderer::getGlobalRenderer()->getWindow()->setVSync(true);

	// Register input callbacks
	renderer::getGlobalRenderer()->getWindow()->registerKeyboardCallback(std::bind(&Game::keyCallback, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    renderer::getGlobalRenderer()->getWindow()->registerMouseCallback(std::bind(&Game::mouseCallback, this,
        std::placeholders::_1, std::placeholders::_2));

	world = std::make_shared<world::World>();

	player = std::make_shared<Player>(world, world::Coord(8, 20, 8), glm::vec3(1, 2, 1));

	world->loadArea(player->getPosition());

	// Create all the text for the game
	textChunkCoords.initialize("Chunk coord:", 25.0f, 200.0f, 0.5f, glm::vec3(1, 1, 1));
	textKeysPressed.initialize("Keys:", 25.0f, 175.0f, 0.5f, glm::vec3(1, 1, 1));
	textCameraDirection.initialize("Camera direction:", 25.0f, 150.0f, 0.5f, glm::vec3(1, 1, 1));
	textCameraPosition.initialize("Camera position:", 25.0f, 125.0f, 0.5f, glm::vec3(1, 1, 1));
	textPlayerDirection.initialize("Player direction:", 25.0f, 100.0f, 0.5f, glm::vec3(1, 1, 1));
	textPlayerPosition.initialize("Player position:", 25.0f, 75.0f, 0.5f, glm::vec3(1, 1, 1));
	textMsPerFrame.initialize("ms per frame:", 25.0f, 50.0f, 0.5f, glm::vec3(1, 1, 1));
	textFps.initialize("fps:", 25.0f, 25.0f, 0.5f, glm::vec3(1, 1, 1));

	running = true;
}

Game::~Game()
{}

void
Game::stop()
{
	renderer::getGlobalRenderer()->getWindow()->requestClose();

	running = false;
}

bool
Game::isRunning()
{
	return running;
}

void Game::doInput(GLfloat deltaTime)
{
	if (keys[GLFW_KEY_W]) {
		player->processKeyboardInput(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S]) {
		player->processKeyboardInput(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A]) {
		player->processKeyboardInput(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D]) {
		player->processKeyboardInput(RIGHT, deltaTime);
	}
	if (keys[GLFW_KEY_SPACE]) {
		player->processKeyboardInput(JUMP, deltaTime);
		keys[GLFW_KEY_SPACE] = false;
	}
	if (keys[GLFW_KEY_F]) {
		player->processKeyboardInput(FLY_TOGGLE, deltaTime);
		keys[GLFW_KEY_F] = false;
	}
}

void Game::doUpdate(GLfloat deltaTime)
{
	player->doUpdate(deltaTime);

	renderer::getGlobalRenderer()->updateCamera(player->getViewMatrix(), player->getPosition());

	// Dashboard update
	textKeysPressed.updateText(fmt::format("Keys: W={} A={} S={} D={}", keys[GLFW_KEY_W], keys[GLFW_KEY_A], keys[GLFW_KEY_S], keys[GLFW_KEY_D]));
	textCameraDirection.updateText(fmt::format("Camera direction: x={}, y={} z={}", player->getCameraDirection().x, player->getCameraDirection().y, player->getCameraDirection().z));
	textCameraPosition.updateText(fmt::format("Camera position: {}", player->getCameraPosition()));
	textPlayerDirection.updateText(fmt::format("Player direction: x={} y={} z={}", player->getDirection().x, player->getDirection().y, player->getDirection().z));
	textPlayerPosition.updateText(fmt::format("Player position: {}", player->getPosition()));
}

void Game::updateFPS(std::string fpsStr, std::string msStr)
{
	textMsPerFrame.updateText(msStr);
	textFps.updateText(fpsStr);
}

void Game::keyCallback(int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		stop();
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}

void Game::mouseCallback(double xPos, double yPos)
{
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	player->processMouseInput(xOffset, yOffset);
}
