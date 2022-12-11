#pragma once

#include "AABB.hpp"

class Ball {

public:

	float x, y;

	AABB aabb;

	Ball() {
		aabb.x = &x;
		aabb.y = &y;
	}
};