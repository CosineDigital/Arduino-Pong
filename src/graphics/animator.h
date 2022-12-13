#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include <vector>
#include <array>

template<size_t N>
class Animator
{
public:

	Animator() = default;

	// to save memory, limit of 255 frames in an animation
	using idxType = uint32_t;

	/**
	* @param sprites - The sprites to be used in each frame of the animation 
	* @param speed - The number of frames between the animation should be cycled; 
	*	ie. before the next sprite in the animation should show 
	* @param position - The starting frame of the animation when created; default = 0
	*/
	Animator(std::array<uint32_t, N> sprites, idxType cycleSpeed, idxType position = 0)
	{
		mSprites = std::move(sprites);
		lastFrameIdx = (idxType)mSprites.size() - (idxType)1;

		mCycleSpeed = cycleSpeed;
		currentFrameIdx = position;
	}

	// update the animation, should be called each frame
	void update() noexcept
	{
		// cycle the animation
		if (++numFrames > mCycleSpeed) {
			numFrames = (idxType)0;
			currentFrameIdx++;
		}

		// Reset the animation
		if (currentFrameIdx > lastFrameIdx) {
			currentFrameIdx = (idxType)0;
		}
	}

	inline void reset() noexcept {
		currentFrameIdx = 0;
	}

	// Returns a const reference to the current spriteIndex in the animation
	inline const uint32_t& current() const noexcept
	{
		return mSprites[(int)currentFrameIdx];
	}

private:
	std::array<uint32_t, N> mSprites;

	idxType mCycleSpeed;

	idxType numFrames{0};
	idxType lastFrameIdx;

	idxType currentFrameIdx;
};

#endif // !ANIMATOR_H_

