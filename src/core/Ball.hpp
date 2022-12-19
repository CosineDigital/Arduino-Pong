#pragma once

#include "rect.hpp"

class Ball final : public Rect {

public:

	glm::vec2 vel;

	Ball() {
		vel = {};
	}

	CollisionDirection checkCollisionSwept(Rect* other, float ts) const noexcept {

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

		// dir in y going up
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
			// there is no collision
			return CollisionDirection::NONE;
		}
		else {
			if (txentry > tyentry) {
				if (dxEntry > 0) {
					return CollisionDirection::LEFT;

				}
				else {
					return CollisionDirection::RIGHT;

				}
			}
			else { // tyentry < txentry
				if (dyEntry > 0) {
					return CollisionDirection::BOTTOM;
				}
				else {
					return CollisionDirection::TOP;
				}
			}
		}
	}

	CollisionDirection checkCollisionAABB(Rect* other) {

		float left = other->left() - this->right();
		float right = other->right() - this->left();
		float top = other->top() - this->bottom();
		float bottom = other->bottom() - this->top();

		return !(left > 0 || right < 0 || top < 0 || bottom > 0) ? 
			CollisionDirection::UNKNOWN : CollisionDirection::NONE;
	}
};