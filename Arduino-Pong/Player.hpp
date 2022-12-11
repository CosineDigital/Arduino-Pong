#pragma once

#include "AABB.hpp"

class Player final : public AABB  {

public:
	Player();

	int score;
	float dy;

private:

};