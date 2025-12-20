#include "game.h"

Game::Game()
{
	renderer::getGlobalRenderer()->updateFOV(45.0f);
	renderer::getGlobalRenderer()->getWindow()->setVSync(true);

	// Register input callbacks
	renderer::getGlobalRenderer()->getWindow()->registerKeyboardCallback(std::bind(&keyCallback, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    renderer::getGlobalRenderer()->getWindow()->registerMouseCallback(std::bind(&mouseCallback, this,
        std::placeholders::_1, std::placeholders::_2));

	world = new world::World();

	player = new Player(world, world::Coord(8, 20, 8), glm::vec3(1, 2, 1));

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
}

Game::~Game()
{}

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
}

void Game::doRender() {
	//std::string playerInfo = player->doUpdate();

	//text->RenderText(playerInfo, 25.0f, 225.0f, 1.0f, glm::vec3(1, 1, 1));

	std::ostringstream keyPressed, chunkCoord;
	/*world::Chunk* chunk = world->getChunkByCoords(player->getPosition().x, player->getPosition().z);
	if (chunk == nullptr) {
		chunkCoord << "Chunk coord: NULL";
	}
	else {
		chunkCoord << "Chunk coord: x: " << chunk->getXPos() << " z: " << chunk->getZPos();
	}
	textChunkCoords.updateText(chunkCoord.str());*/
	keyPressed << "Keys: W: " << keys[GLFW_KEY_W] << " S: " << keys[GLFW_KEY_S] << " A: " << keys[GLFW_KEY_A] << " D: " << keys[GLFW_KEY_D];
	textKeysPressed.updateText(keyPressed.str());

	std::ostringstream playerPos, playerDir, camPos, camDir;
	camDir << "Camera direction: x: " << player->getCameraDirection().x << " y: " << player->getCameraDirection().y << " z: " << player->getCameraDirection().z;
	textCameraDirection.updateText(camDir.str());
	camPos << "Camera position: x: " << player->getCameraPosition().x << " y: " << player->getCameraPosition().y << " z: " << player->getCameraPosition().z;
	textCameraPosition.updateText(camPos.str());
	playerDir << "Player direction: x: " << player->getDirection().x << " y: " << player->getDirection().y << " z: " << player->getDirection().z;
	textPlayerDirection.updateText(playerDir.str());
	playerPos << "Player position: x: " << player->getPosition().x << " y: " << player->getPosition().y << " z: " << player->getPosition().z;
	textPlayerPosition.updateText(playerPos.str());
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
		renderer::getGlobalRenderer()->getWindow()->requestClose();
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
