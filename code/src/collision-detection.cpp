#include "collision-detection.h"

Collision_Detection::Collision_Detection() {}
Collision_Detection::~Collision_Detection() {}

bool Collision_Detection::cubeToRect(glm::vec2 cubeRangeA, glm::vec2 cubeRangeB, glm::vec2 cubeRangeC, glm::vec2 rectRangeA, glm::vec2 rectRangeB, float rectPosC) {
	return overlaps(cubeRangeA, rectRangeA) && overlaps(cubeRangeB, rectRangeB) && contains(cubeRangeC, rectPosC);
}

bool Collision_Detection::overlaps(glm::vec2 range1, glm::vec2 range2) {
	return contains(range1, range2.x) || contains(range1, range2.y);
}

bool Collision_Detection::contains(glm::vec2 range, float point) {
	return range.x < point && range.y > point;
}