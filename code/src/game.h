#pragma once

#include <vector>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shaders/block.h"
#include "world.h"
#include "textures/loader.h"
#include "text.h"

#include "player.h"

class Game {
private:
	//Camera camera;
	std::shared_ptr<textures::Loader> texLoader;
	std::shared_ptr<shaders::Block> blockShader;
	World* world;

	Player* player;

	bool keys[1024] = { false };
	GLfloat lastX;
	GLfloat lastY;
	bool firstMouse = true;

	void loadShaders(int screenWidth, int screenHeight);
	void loadTextures();

public:
	Game(int screenWidth, int screenHeight);
	~Game();

	void doInput(GLfloat deltaTime);
	void doUpdate(GLfloat deltaTime);
	void doRender(Text* text);

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void mouseCallback(GLFWwindow* window, double xPos, double yPos);

	void updateProjection(GLfloat fov, int screenWidth, int screenHeight);
};
