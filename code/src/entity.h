#pragma once

#include <ostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "collision-detection.h"
#include "world.h"
#include "block.h"

class Entity {
public:
	Entity();
	Entity(World* world, glm::vec3 position, glm::vec3 dimentions, GLfloat yaw = 0.0f, GLfloat pitch = 0.0f, bool flying = false, float speed = 6.0f, float jumpSpeed = 6.0f);
	~Entity();

	virtual void doUpdate(GLfloat deltaTime);

	glm::vec3 getPosition();
	glm::vec3 getDirection();
	float getX();
	float getY();
	float getZ();
	
protected:
	World* world;

	glm::vec3 position, dimentions, front, frontHor, worldUp, up, right;
	glm::vec3 potentialPos, velocity;
	GLfloat yaw, pitch;
	float speed, jumpSpeed;
	bool flying;

	Collision_Detection collisionDetection;

	virtual void updateVectors();
};