#ifndef SPRITE_HANDLE_H_
#define SPRITE_HANDLE_H_

#include "C:\C++ Libraries\glm-0.9.9.8\glm\glm.hpp"

#include <iostream>

#include "sprite_sheet.h"

struct Sprite
{
	inline unsigned int getWidth(void) const noexcept
	{
		return this->width;
	}

	inline unsigned int getHeight(void) const noexcept
	{
		return this->height;
	}

	/** store all the variables here 
	* instead of re-calculating each frame
	*/
	glm::vec2 topLeft;
	glm::vec2 topRight;
	glm::vec2 bottomLeft;
	glm::vec2 bottomRight;

	unsigned int width, height;
};

/**
*
* (0, 0) in image editor, the origin
*
*   top left  -----------   top right
*		      |			|  ^
*		      |			|Height
*			  | <Width> |  V
* bottom left -----------  bottom right
* 
*/

/**
* @brief Creates a sprite given a sprite sheet
* @param spriteSheet - The SpriteSheet to source from ; sprites are unique to a single SpriteSheet
* @param origin - The origin of the image in an image editor, usually the top left (in pixels)
* @param width - The desired width of the sprite (in pixels) ; default = 16 pixels
* @param height - The desired height of the sprite (in pixels) ; default = 16 pixels
*/
static Sprite createSprite(const SpriteSheet* spriteSheet, int x, int y,
	int width = 16u, int height = 16u) noexcept
{
	float sheetWidth = (float)spriteSheet->getWidth();
	float sheetHeight = (float)spriteSheet->getHeight();

	Sprite sprite;
	/** @note - all coordinates range from 0.0f to 1.0f */

	// top left
	sprite.topLeft = { (float)x / sheetWidth, (float)y / sheetHeight };

	// bottom left
	sprite.bottomLeft = { (float)x / sheetWidth, ((float)y + (float)height) / sheetHeight };
	
	// top right
	sprite.topRight = { ((float)x + (float)width) / sheetWidth, (float)y / sheetHeight };

	// bottom right
	sprite.bottomRight = { ((float)x + (float)width) / sheetWidth, ((float)y + (float)height) / sheetHeight };

	// width and height
	sprite.width = width;
	sprite.height = height;

	return sprite;
}

/**
* @brief Creates a sprite given a sprite sheet and coordinates
* @note - A sprite at coordinates 48, 16 should be recieving (x, y) pair of (3, 1)
* @param spriteSheet - The SpriteSheet to source from ; sprites are unique to a single SpriteSheet
* @param x - The x coordinate of the sprite
* @param y - The y coordinate of the sprite
*/
static Sprite createSpriteAuto(const SpriteSheet& spriteSheet, unsigned int x, unsigned int y) noexcept
{
	Sprite sprite;

	float sheetWidth = (float)spriteSheet.getWidth();
	float sheetHeight = (float)spriteSheet.getHeight();

	// top left
	sprite.topLeft = { (x * 16.0f) / sheetWidth, (y * 16.0f) / sheetHeight };

	// bottom left
	sprite.bottomLeft = { (x * 16.0f) / sheetWidth, ((y * 16.0f) + (float)16) / sheetHeight };

	// top right
	sprite.topRight = { ((x * 16.0f) + (float)16) / sheetWidth, (y * 16.0f) / sheetHeight };

	// bottom right
	sprite.bottomRight = { ((x * 16.0f) + (float)16) / sheetWidth, ((y * 16.0f) + (float)16) / sheetHeight };

	// width and height
	sprite.width = 16;
	sprite.height = 16;

	return sprite;
}

#endif // SPRITE_HANDLE_H_
