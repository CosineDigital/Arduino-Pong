#ifndef PARTICLE_H_
#define PARTICLE_H_

#include "renderer.h"

class Particle
{
public:
	Particle(glm::vec2 origin, uint32_t sprite, uint32_t duration, glm::vec2(*lambda)(uint32_t))
	{
		this->position = origin;
		this->sprite = sprite;
		this->duration = duration;
		this->velocityFunction = lambda;
	}

	inline void update() noexcept
	{
		position += velocityFunction(duration--);
	}

	inline void draw(Renderer* renderer) const noexcept
	{
		//renderer->buffer(this->position, this->sprite);
	}
private:

	glm::vec2 position;
	uint32_t sprite;
	uint32_t duration;
	glm::vec2(*velocityFunction)(uint32_t);

};

#endif // !PARTICLE_H_