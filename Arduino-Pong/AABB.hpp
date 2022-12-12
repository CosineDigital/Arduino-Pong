#pragma once

#include <functional>

class AABB {
public:
	float x, y;
	float width, height;

	inline float right() {

	}
};

enum class ColDir {
	NONE = 0,
	RIGHT,	
	TOP,
	LEFT,
	BOTTOM
};