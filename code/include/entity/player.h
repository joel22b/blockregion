#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "entity/entity.h"
#include "utils/camera.h"

enum Player_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	JUMP,
	FLY_TOGGLE
};

class Player: public Entity {
public:
	Player(std::shared_ptr<world::World> world, world::Coord position, glm::vec3 dimentions, GLfloat yaw = 0.0f, GLfloat pitch = 0.0f, bool flying = true, float speed = 6.0f, float jumpSpeed = 6.0f);

	void processKeyboardInput(Player_Movement movement, GLfloat deltaTime);
	void processMouseInput(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true);

	void doUpdate(GLfloat deltaTime) override;

	glm::mat4 getViewMatrix();

	world::Coord getCameraPosition();
	glm::vec3 getCameraDirection();

private:
	std::unique_ptr<Camera> camera;
	GLfloat mouseSensitivity = 0.25f;
	glm::vec2 chunkCoords;

	void updateVectors() override;
};