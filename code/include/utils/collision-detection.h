#pragma once

#include "glm/glm.hpp"

class Collision_Detection {
public:
	Collision_Detection();
	~Collision_Detection();

	bool cubeToRect(glm::vec2 cubeRangeA, glm::vec2 cubeRangeB, glm::vec2 cubeRangeC, glm::vec2 rectRangeA, glm::vec2 rectRangeB, float rectPosC);

private:
	bool overlaps(glm::vec2 range1, glm::vec2 range2);
	bool contains(glm::vec2 range, float point);
};