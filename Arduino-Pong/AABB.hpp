#pragma once

#include <functional>

class AABB {
public:
	float x, y;
	float width, height;
};

enum class ColDir {
	NONE = 0,
	RIGHT,	
	TOP,
	LEFT,
	BOTTOM
};

// object collides with other
ColDir isColliding(AABB* object, AABB* other) {

	


}