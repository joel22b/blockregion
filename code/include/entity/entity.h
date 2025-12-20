#pragma once

#include <ostream>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "utils/collision-detection.h"
#include "world/world.h"
#include "world/block.h"

class Entity {
public:
	Entity(std::shared_ptr<world::World> world, world::Coord position, glm::vec3 dimentions, GLfloat yaw = 0.0f, GLfloat pitch = 0.0f, bool flying = false, float speed = 6.0f, float jumpSpeed = 6.0f);
	~Entity();

	virtual void doUpdate(GLfloat deltaTime);

	world::Coord getPosition();
	glm::vec3 getDirection();
	float getX();
	float getY();
	float getZ();
	
protected:
	std::shared_ptr<world::World> world;

	world::Coord position;
	glm::vec3 dimentions, front, frontHor, worldUp, up, right;
	glm::vec3 potentialPos, velocity;
	GLfloat yaw, pitch;
	float speed, jumpSpeed;
	bool flying;

	Collision_Detection collisionDetection;

	virtual void updateVectors();
};