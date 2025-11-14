#include "player.h"

//#include "../../utils/Logger.h"
//#define LOG(severity, msg) Logger::log("Player.cpp", severity, msg)

Player::Player() : Entity() {
	camera = nullptr;
}

Player::Player(world::World* world, glm::vec3 position, glm::vec3 dimentions, GLfloat yaw, GLfloat pitch, bool flying, float speed, float jumpSpeed) : Entity(world, position, dimentions, yaw, pitch, flying, speed, jumpSpeed) {
	camera = new Camera(position);
	camera->setCameraVectors(position + glm::vec3(0, 1, 0), front, right, up);

	chunkCoords = world->getChunkCoords((int)position.x, (int)position.z);
}

void Player::processKeyboardInput(Player_Movement movement, GLfloat deltaTime) {
	float velocity = this->speed * deltaTime;

	if (movement == FORWARD) {
		this->potentialPos += this->front * velocity;
	}
	if (movement == BACKWARD) {
		this->potentialPos -= this->front * velocity;
	}
	if (movement == LEFT) {
		this->potentialPos -= this->right * velocity;
	}
	if (movement == RIGHT) {
		this->potentialPos += this->right * velocity;
	}
	if (movement == JUMP) {
		this->velocity += this->worldUp * this->jumpSpeed;
	}
	if (movement == FLY_TOGGLE) {
		this->flying = !flying;
	}
}

void Player::processMouseInput(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch) {
	xOffset *= this->mouseSensitivity;
	yOffset *= this->mouseSensitivity;

	this->yaw += xOffset;
	this->pitch += yOffset;

	if (constrainPitch) {
		if (this->pitch > 89.0f) {
			this->pitch = 89.0f;
		}
		if (this->pitch < -89.f) {
			this->pitch = -89.0f;
		}
	}

	updateVectors();
}

void Player::doUpdate(GLfloat deltaTime) {
	// Do regular entity update
	Entity::doUpdate(deltaTime);

	// Check if changed chunks
	if (chunkCoords != world->getChunkCoords((int)position.x, (int)position.z)) {
		chunkCoords = world->getChunkCoords((int)position.x, (int)position.z);
		world->shiftChunks((int)chunkCoords.x, (int)chunkCoords.y);
	}
}

glm::mat4 Player::getViewMatrix() {
	return camera->getViewMatrix();
}

glm::vec3 Player::getCameraPosition() {
	return camera->getPosition();
}

glm::vec3 Player::getCameraDirection() {
	return camera->getViewDirection();
}

void Player::updateVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));

	camera->setCameraVectors(position + glm::vec3(0, 1, 0), front, right, up);
}