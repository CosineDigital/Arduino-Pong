#ifndef SPRITE_SHEET_H_
#define SPRITE_SHEET_H_

#include <cassert>
#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

class SpriteSheet {
public:
    SpriteSheet() = default;

    SpriteSheet(unsigned int id, unsigned int width, unsigned int height)
    {
        mID = id;
        mWidth = width;
        mHeight = height;
    }

    ~SpriteSheet() noexcept {
        glDeleteTextures(1, &mID);
    }

    inline const unsigned int& getID(void) const noexcept {
        return mID;
    }

    inline const unsigned int& getWidth(void) const noexcept {
        return mWidth;
    }

    inline const unsigned int& getHeight(void) const noexcept {
        return mHeight;
    }

private:
    // do not modify these after assignment
    unsigned int mID;
    unsigned int mWidth;
    unsigned int mHeight;
};

/**
* @brief Creates a SpriteSheet from an image
* @param file - The full path to the image's source relative to the .exe file
* @param flipVertically - whether to flip the image when loading
*/
static SpriteSheet loadSpriteSheet(const char* path, int flipVertically = 0) noexcept
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // sprites should clamp to edge if invalid texCoords are set
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // sprites should stay pixelated
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, colorChannels;
    stbi_set_flip_vertically_on_load(flipVertically);
    unsigned char* buffer = stbi_load(path, &width, &height, &colorChannels, 0);
    
    if (buffer)
    {
        GLenum format = GL_RGBA;
        switch (colorChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            std::cout << "Invalid format from " << path << '\n';
            assert(false);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        stbi_image_free(buffer);
    }
    else
    {
        std::cout << "Could not load texture from " << path << '\n';
        stbi_image_free(buffer);
    }
    // width, height (x, y);
    return SpriteSheet(textureID, width, height);
}

#endif // !SPRITE_SHEET_H_
