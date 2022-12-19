#pragma once
#include <functional>
#include <glm/glm.hpp>

class AABB {
public:
	bool is_trigger;
	bool is_player;
	std::function<void()> trigger_callback;

	AABB() {
		is_trigger = false;
		is_player = false;
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