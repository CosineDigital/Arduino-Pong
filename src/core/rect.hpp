#pragma once
#include <functional>
#include <glm/glm.hpp>

class Rect {
public:
	bool is_trigger;
	bool is_player;
	std::function<void()> trigger_callback;

	Rect() {
		is_trigger = false;
		is_player = false;
		dim = {};
		pos = {};
	}

	glm::vec2 pos, dim;

	inline float top() const noexcept {
		return pos.y + dim.y;
	}

	inline float right() const noexcept {
		return pos.x + dim.x;
	}

	inline float bottom() const noexcept {
		return pos.y - dim.y;
	}

	inline float left() const noexcept {
		return pos.x - dim.x;
	}

};


enum class CollisionDirection {
	NONE,
	RIGHT,
	TOP,
	LEFT,
	BOTTOM,
	UNKNOWN
};