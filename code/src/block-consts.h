#pragma once

#include <iostream>
#include <string>
#include <ostream>
#include <sstream>
#include <unordered_map>

#include "glm/glm.hpp"

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

class Block_Consts {
public:

	Block_Consts() {
		loadBlockTexCoords();
	}

	glm::vec2 getBlockTexCoords(Block_Type type, Block_Side dir) {
		std::ostringstream query;
		query << type << "_" << dir;
		return blockTexCoords[query.str()];
	}

private:
	std::unordered_map<std::string, glm::vec2> blockTexCoords;

	void loadBlockTexCoords() {
		// GRASS
		blockTexCoords["1_0"] = glm::vec2(1.0f / 4.0f, 0.0f);
		blockTexCoords["1_1"] = glm::vec2(1.0f / 4.0f, 0.0f);
		blockTexCoords["1_2"] = glm::vec2(0.0f / 4.0f, 0.0f);
		blockTexCoords["1_3"] = glm::vec2(2.0f / 4.0f, 0.0f);
		blockTexCoords["1_4"] = glm::vec2(1.0f / 4.0f, 0.0f);
		blockTexCoords["1_5"] = glm::vec2(1.0f / 4.0f, 0.0f);
		
		// DIRT
		blockTexCoords["2_0"] = glm::vec2(2.0f / 4.0f, 0.0f);
		blockTexCoords["2_1"] = glm::vec2(2.0f / 4.0f, 0.0f);
		blockTexCoords["2_2"] = glm::vec2(2.0f / 4.0f, 0.0f);
		blockTexCoords["2_3"] = glm::vec2(2.0f / 4.0f, 0.0f);
		blockTexCoords["2_4"] = glm::vec2(2.0f / 4.0f, 0.0f);
		blockTexCoords["2_5"] = glm::vec2(2.0f / 4.0f, 0.0f);

		// TEST
		blockTexCoords["3_0"] = glm::vec2(3.0f / 4.0f, 0.0f);
		blockTexCoords["3_1"] = glm::vec2(3.0f / 4.0f, 0.0f);
		blockTexCoords["3_2"] = glm::vec2(3.0f / 4.0f, 0.0f);
		blockTexCoords["3_3"] = glm::vec2(3.0f / 4.0f, 0.0f);
		blockTexCoords["3_4"] = glm::vec2(3.0f / 4.0f, 0.0f);
		blockTexCoords["3_5"] = glm::vec2(3.0f / 4.0f, 0.0f);
	}
};