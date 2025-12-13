#include "entity/entity.h"

Entity::Entity(world::World* world, world::Coord position, glm::vec3 dimentions, GLfloat yaw, GLfloat pitch, bool flying, float speed, float jumpSpeed):
	position(position)
{
	this->world = world;
	this->dimentions = dimentions;
	this->worldUp = glm::vec3(0, 1, 0);
	this->yaw = yaw;
	this->pitch = pitch;
	this->speed = speed;
	this->jumpSpeed = jumpSpeed;
	this->potentialPos = position.getVec();
	this->flying = flying;

	velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	updateVectors();
}

Entity::~Entity() {
	//LOG(INFO, "Deleting");
}

void Entity::doUpdate(GLfloat deltaTime) {
	// Update velocity
	if (!flying) {
		this->velocity -= this->worldUp * (9.8f * deltaTime);	// Gravity

		this->potentialPos += this->velocity * deltaTime;

		velocity.x = velocity.x * (0.9f * deltaTime);
		velocity.z = velocity.z * (0.9f * deltaTime);
	}
	else {
		this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	// Collision Detection
	glm::vec2 entityPos[] = { glm::vec2(potentialPos.x - (dimentions.x / 2), potentialPos.x + (dimentions.x / 2)),
		glm::vec2(potentialPos.y, potentialPos.y + dimentions.y), glm::vec2(potentialPos.z - (dimentions.z / 2), potentialPos.z + (dimentions.z / 2)) };
	bool collided = false;
	glm::vec3 posInt = glm::vec3(glm::floor(position.x), glm::floor(position.y), glm::floor(position.z));

	glm::vec4 collisionPosFaces[] = {
		glm::vec4(-1, 0, 0, world::XPOS), glm::vec4(-1, 1, 0, world::XPOS),

		glm::vec4(1, 0, 0, world::XNEG), glm::vec4(1, 1, 0, world::XNEG),

		glm::vec4(0, -1, 0, world::YPOS), glm::vec4(1, -1, 0, world::YPOS),

		glm::vec4(-1, -1, 0, world::YPOS), glm::vec4(0, -1, 1, world::YPOS),

		glm::vec4(0, -1, -1, world::YPOS),

		glm::vec4(0, 2, 0, world::YNEG),

		glm::vec4(0, 0, -1, world::ZPOS), glm::vec4(0, 1, -1, world::ZPOS),

		glm::vec4(0, 0, 1, world::ZNEG), glm::vec4(0, 1, 1, world::ZNEG)
	};

	for (int i = 0; i < 14; i++) {
		world::Block* block = world->getBlock(world::Coord(posInt.x + collisionPosFaces[i].x, posInt.y + collisionPosFaces[i].y, posInt.z + collisionPosFaces[i].z));

		if (block != nullptr && block->getType() != world::AIR) {
			glm::vec2 blockPos[] = {
				glm::vec2(posInt.x + collisionPosFaces[i].x, posInt.x + collisionPosFaces[i].x + 1),
				glm::vec2(posInt.y + collisionPosFaces[i].y, posInt.y + collisionPosFaces[i].y + 1),
				glm::vec2(posInt.z + collisionPosFaces[i].z, posInt.z + collisionPosFaces[i].z + 1)
			};

			int blockPoint;
			int side = collisionPosFaces[i].w / 2;
			if ((int)collisionPosFaces[i].w % 2 == 0) {
				blockPoint = blockPos[side].y;
			}
			else {
				blockPoint = blockPos[side].x;
			}

			if (collisionDetection.cubeToRect(entityPos[(side + 1) % 3], entityPos[(side + 2) % 3], entityPos[side % 3],
				blockPos[(side + 1) % 3], blockPos[(side + 2) % 3], blockPoint)) {
				collided = true;
				if (side == 0) {
					potentialPos.x = position.x;
					velocity.x = 0.0f;
				}
				else if (side == 1) {
					potentialPos.y = position.y;
					velocity.y = 0.0f;
				}
				else {
					potentialPos.z = position.z;
					velocity.z = 0.0f;
				}
			}
		}
	}

	position = potentialPos;
	updateVectors();
}

world::Coord
Entity::getPosition()
{
	return position;
}

glm::vec3 Entity::getDirection() {
	return front;
}

float Entity::getX() {
	return position.x;
}

float Entity::getY() {
	return position.y;
}

float Entity::getZ() {
	return position.z;
}

void Entity::updateVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}