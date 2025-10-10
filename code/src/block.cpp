#include "block.h"

//#include "../../utils/Logger.h"
//#define LOG(severity, msg) Logger::log("Block.cpp", severity, msg)

Block::Block(Block_Type type) {
	this->type = type;
}

Block::~Block() {
	this->type = AIR;
}

Block_Type Block::getType() {
	return this->type;
}

void Block::setType(Block_Type type) {
	this->type = type;
}