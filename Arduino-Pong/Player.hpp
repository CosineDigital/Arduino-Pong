#pragma once

#include "AABB.hpp"

class Player final {

public:
	int score;
	float xpos, ypos;
	float dy;

	AABB aabb;

	Player() {
		aabb.x = &ypos;
		aabb.y = &xpos;
	}

private:

};