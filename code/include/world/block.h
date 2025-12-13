#pragma once

#include <vector>
#include <iostream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace world
{

enum Block_Type {
	AIR = 0,
	GRASS = 1,
	DIRT = 2,
	TEST = 3
};

enum Block_Side {
	XPOS = 0,
	XNEG = 1,
	YPOS = 2,
	YNEG = 3,
	ZPOS = 4,
	ZNEG = 5
};

const int BLOCK_WIDTH = 1;

class Block {
private:
	Block_Type type;

public:
	Block(Block_Type type = AIR);
	~Block();

	Block_Type getType();
	void setType(Block_Type newType);
};

/********************************
 * Definitions
********************************/

inline
Block::Block(Block_Type type) : type(type)
{}

inline
Block::~Block()
{}

inline
Block_Type
Block::getType()
{
	return type;
}

inline
void
Block::setType(Block_Type newType)
{
	type = newType;
}

} // namespace world
