#pragma once
#include <functional>
#include <glm/glm.hpp>

class AABB {
public:
	bool is_trigger;
	std::function<void()> trigger_callback;

	AABB() {
		is_trigger = false;
		dim = {};
		pos = {};
	}

	glm::vec2 pos, dim;
};


enum class CollisionDirection {
	NONE,
	RIGHT,
	TOP,
	LEFT,
	BOTTOM
};

struct Collision {
	CollisionDirection direction;
	float time;
};