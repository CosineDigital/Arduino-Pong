#pragma once

#include "AABB.hpp"

class Ball final : public AABB {

public:

	float dx, dy;

	AABB aabb;

	Ball() {};

	ColDir isColliding(AABB* other) {

		// other.left < object.right
		auto flag1 = other->x - other->width < this->x + this->width;

		//other.rigth > object.left
		auto flag2 = other->x + other->width > this->x - this->width;

		// other.top > object.bottom
		auto flag3 = other->y + other->height > this->y - this->height;

		// other.bottom < other.top
		auto flag4 = other->y - other->height < this->y + this->height;

		// if there is a collision
		if (flag1 && flag2 && flag3 && flag4) {
			// objects are colliding

			// v dot n < 0 -> ball moves towards the left or top faces
			if (dx * -1 + dy * 0 < 0 || dx * 0 + dy * 1 < 0) {
				// dist from ball to left face
				auto hdist = this->x + this->width - (other->x - other->width);
				// dist from the ball to the top face
				auto vdist = this->y - this->height - (other->y + other->height);

				// if hdist is greatest, then we have collided from the top
				return std::abs(hdist) > std::abs(vdist) ? ColDir::TOP : ColDir::LEFT;
			}
			// v dot n > 0 -> ball moves towards the bottom or right faces
			else {
				// dist from ball to right face
				auto hdist = this->x - this->width - (other->x + other->width);
				// dist from the ball to the bottom face
				auto vdist = this->y + this->height - (other->y - other->height);

				// if hdist is greatest, then we have collided from the bottom
				return std::abs(hdist) > std::abs(vdist) ? ColDir::BOTTOM : ColDir::RIGHT;
			}
		}
		else {
			return ColDir::NONE;
		}
	}
};