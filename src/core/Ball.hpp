#pragma once

#include "AABB.hpp"

class Ball final : public AABB {

public:

	glm::vec2 vel;

	Ball() {
		vel = {};
	}

	Collision checkCollision(AABB* other, float ts) const noexcept {

		// distance to entry and exit
		float dxEntry, dyEntry;
		float dxExit, dyExit;

		// dir in x going to right
		if (this->vel.x > 0) {
			dxEntry = -(pos.x + dim.x) + (other->pos.x - other->dim.x);
			dxExit = -(pos.x - dim.x) + (other->pos.x + other->dim.x);
		}
		else
		{
			dxEntry = (other->pos.x + other->dim.x) - (pos.x - dim.x);
			dxExit = (other->pos.x - other->dim.x) - (pos.x + dim.x);
		}

		// dir in y going to left
		if (this->vel.y > 0) {
			dyEntry = +(other->pos.y - other->dim.y) - (pos.y + dim.y);
			dyExit = +(other->pos.y + other->dim.y) - (pos.y - dim.y);
		}
		else { // going down
			dyEntry = +(other->pos.y + other->dim.y) - (pos.y - dim.y);
			dyExit = +(other->pos.y - other->dim.y) - (pos.y + dim.y);
		}

		float txentry, tyentry;
		float txexit, tyexit;

		if (vel.x == 0) {
			txentry = -std::numeric_limits<float>::infinity();
			txexit = std::numeric_limits<float>::infinity();
		}
		else {
			txentry = dxEntry / vel.x;
			txexit = dxExit / vel.x;
		}

		if (vel.y == 0) {
			tyentry = -std::numeric_limits<float>::infinity();
			tyexit = std::numeric_limits<float>::infinity();
		}
		else {
			tyentry = dyEntry / vel.y;
			tyexit = dyExit / vel.y;
		}

		float entryTime = std::max(txentry, tyentry) / ts;
		float exitTime = std::min(txexit, tyexit) / ts;

		float time{};

		if (entryTime > exitTime || (txentry < 0 && tyentry < 0) || txentry > ts || tyentry > ts) {
			time = 1;
		}
		else {
			time = entryTime;
		}

		CollisionDirection dir{};

		if (txentry > tyentry) {
			if (dxEntry > 0) {
				dir = CollisionDirection::RIGHT;

			}
			else {
				dir = CollisionDirection::LEFT;

			}
		}
		else {
			if (dyEntry > 0) {
				dir = CollisionDirection::TOP;
			}
			else {
				dir = CollisionDirection::BOTTOM;
			}
		}

		return {dir, time};
	}
};