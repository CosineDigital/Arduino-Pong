#pragma once

#include "AABB.hpp"

class Ball final : public AABB {

public:

	float dx, dy;

	AABB aabb;

	Ball();
};