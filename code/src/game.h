#pragma once

#include <vector>
#include <atomic>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/renderer.h"

#include "text/text.h"

#include "utils/camera.h"
#include "shaders/block.h"
#include "world/world.h"
#include "textures/loader.h"

#include "entity/player.h"

class Game {
private:
	std::shared_ptr<world::World> world;

	std::shared_ptr<Player> player;

	bool keys[1024] = { false };
	GLfloat lastX;
	GLfloat lastY;
	bool firstMouse = true;

	text::Text textChunkCoords;
	text::Text textKeysPressed;
	text::Text textCameraDirection;
	text::Text textCameraPosition;
	text::Text textPlayerDirection;
	text::Text textPlayerPosition;
	text::Text textMsPerFrame;
	text::Text textFps;

	std::atomic<bool> running {false};

public:
	Game();
	~Game();

	void stop();
	bool isRunning();

	void doInput(GLfloat deltaTime);
	void doUpdate(GLfloat deltaTime);
	
	void updateFPS(std::string fpsStr, std::string msStr);

	void keyCallback(int key, int scancode, int action, int mode);
	void mouseCallback(double xPos, double yPos);
};
