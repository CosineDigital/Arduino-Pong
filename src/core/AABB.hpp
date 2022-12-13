#pragma once
#include <functional>
#include "C:\C++ Libraries\glm-0.9.9.8\glm\glm.hpp"
#include "Ball.hpp"

class Ball;

class AABB {
public:
	bool is_trigger;
	std::function<void()> trigger_callback;

	AABB() {
		is_trigger = false;
		dim = {};
		pos = {};
	}

	glm::vec2 pos;
	glm::vec2 dim;
};

enum class ColDir {
	NONE,
	RIGHT,	
	TOP,
	LEFT,
	BOTTOM
};