#include "game.h"

//#include "utils/Logger.h"
//#define LOG(severity, msg) Logger::log("Game.cpp", severity, msg)

Game::Game(int screenWidth, int screenHeight) {
	//LOG(INFO, "Creating Game instance");

	texLoader = std::make_shared<textures::Loader>();
	blockShader = std::make_shared<shaders::Block>(texLoader);

	loadShaders(screenWidth, screenHeight);

	world = new World(blockShader);

	player = new Player(world, glm::vec3(8.0f, 20.0f, 8.0f), glm::vec3(1, 2, 1));

	world->updateChunkRenderDistance(2, 1, player->getX(), player->getZ());

	//LOG(INFO, "Finished creating Game instance");
}

Game::~Game() {
	//LOG(INFO, "Deleting");
}

void Game::doInput(GLfloat deltaTime) {
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

void Game::doUpdate(GLfloat deltaTime) {
	player->doUpdate(deltaTime);

	blockShader->Use();
	glUniformMatrix4fv(glGetUniformLocation(blockShader->getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(player->getViewMatrix()));
	glUniform3f(glGetUniformLocation(blockShader->getProgram(), "viewPos"), player->getPosition().x, player->getPosition().y, player->getPosition().z);
}

void Game::doRender(Text* text) {
	//std::string playerInfo = player->doUpdate();

	blockShader->Use();
	GLint modelLoc = glGetUniformLocation(blockShader->getProgram(), "model");
	world->doRender(modelLoc);

	//text->RenderText(playerInfo, 25.0f, 225.0f, 1.0f, glm::vec3(1, 1, 1));

	std::ostringstream keyPressed, chunkCoord;
	Chunk* chunk = world->getChunkByCoords(player->getPosition().x, player->getPosition().z);
	if (chunk == nullptr) {
		chunkCoord << "Chunk coord: NULL";
	}
	else {
		chunkCoord << "Chunk coord: x: " << chunk->getXPos() << " z: " << chunk->getZPos();
	}
	text->RenderText(chunkCoord.str(), 25.0f, 200.0f, 1.0f, glm::vec3(1, 1, 1));
	keyPressed << "Keys: W: " << keys[GLFW_KEY_W] << " S: " << keys[GLFW_KEY_S] << " A: " << keys[GLFW_KEY_A] << " D: " << keys[GLFW_KEY_D];
	text->RenderText(keyPressed.str(), 25.0f, 175.0f, 1.0f, glm::vec3(1, 1, 1));

	std::ostringstream playerPos, playerDir, camPos, camDir;
	camDir << "Camera direction: x: " << player->getCameraDirection().x << " y: " << player->getCameraDirection().y << " z: " << player->getCameraDirection().z;
	text->RenderText(camDir.str(), 25.0f, 150.0f, 1.0f, glm::vec3(1, 1, 1));
	camPos << "Camera position: x: " << player->getCameraPosition().x << " y: " << player->getCameraPosition().y << " z: " << player->getCameraPosition().z;
	text->RenderText(camPos.str(), 25.0f, 125.0f, 1.0f, glm::vec3(1, 1, 1));
	playerDir << "Player direction: x: " << player->getDirection().x << " y: " << player->getDirection().y << " z: " << player->getDirection().z;
	text->RenderText(playerDir.str(), 25.0f, 100.0f, 1.0f, glm::vec3(1, 1, 1));
	playerPos << "Player position: x: " << player->getPosition().x << " y: " << player->getPosition().y << " z: " << player->getPosition().z;
	text->RenderText(playerPos.str(),25.0f, 75.0f, 1.0f, glm::vec3(1, 1, 1));
}

void Game::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
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

void Game::mouseCallback(GLFWwindow* window, double xPos, double yPos) {
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

void Game::loadShaders(int screenWidth, int screenHeight) {
	// TODO: make this modular so multiple shaders can be used
	// Load block shader

	blockShader->Use();

	updateProjection(45.0f, screenWidth, screenHeight);
}

void Game::updateProjection(GLfloat fov, int screenWidth, int screenHeight) {
	blockShader->Use();
	glm::mat4 projection(1);
	projection = glm::perspective(45.0f, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(blockShader->getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}
