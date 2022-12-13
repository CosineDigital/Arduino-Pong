#pragma once

#include "AABB.hpp"

class Ball final : public AABB {

public:

	float dx, dy;

	Ball() {
		dy = dx = 0;
	}

	void handleCollision(AABB* other) {

		// if the this is trigger, say a goal, then run the callback
		if (other->is_trigger) {
			other->trigger_callback();
			return;
		}

		float hdist = std::abs(pos.x - other->pos.x);
		float vdist = std::abs(pos.y - other->pos.y);

		// no collision
		if (hdist > dim.x + other->dim.x || vdist > dim.y + other->dim.y) {
			return;
		}

		// check for collisions

		/*
		// other.left < this.right
		auto flag1 = other->pos.x - other->dim.x < this->pos.x + this->dim.x;

		//other.rigth > this.left
		auto flag2 = other->pos.x + other->dim.x > this->pos.x - this->dim.x;

		// other.top > this.bottom
		auto flag3 = other->pos.y + other->dim.y > this->pos.y - this->dim.y;

		// other.bottom < other.top
		auto flag4 = other->pos.y - other->dim.y < this->pos.y + this->dim.y;

		// if there is a collision
		if (flag1 && flag2 && flag3 && flag4) {
			// thiss are colliding

			// v dot n < 0 -> ball moves towards the left or top faces
			if (this->dx * -1 + this->dy * 0 < 0 || this->dx * 0 + this->dy * 1 < 0) {
				// dist from ball to left face
				auto hdist = this->pos.x + this->width - (other->pos.x - other->width);
				// dist from the ball to the top face
				auto vdist = this->pos.y - this->dim.y - (other->pos.y + other->dim.y);

				// if hdist is greatest, then we have collided from the top
				return std::abs(hdist) > std::abs(vdist) ? ColDir::TOP : ColDir::LEFT;
			}
			// v dot n > 0 -> ball moves towards the bottom or right faces
			else {
				// dist from ball to right face
				auto hdist = this->pos.x - this->width - (other->pos.x + other->width);
				// dist from the ball to the bottom face
				auto vdist = this->pos.y + this->dim.y - (other->pos.y - other->dim.y);

				// if hdist is greatest, then we have collided from the bottom
				return std::abs(hdist) > std::abs(vdist) ? ColDir::BOTTOM : ColDir::RIGHT;
			}
		}*/
	}
};