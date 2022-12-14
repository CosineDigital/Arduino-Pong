#include "renderer.hpp"

/**
* @brief Creates the indices for a Quad in an index buffer
* @param count - The place to write the data to in the index buffer
* @param indexData - The index buffer to write to
*/
void Renderer::createQuadIndices() noexcept
{
    /**
    * @note - fill index data
    *                         ____
    * First triangle [0 -> 2]:|  /
    *                         | /
    *                         |/
    */
    indexData[count * 6 + 0] = count * 4 + 0;
    indexData[count * 6 + 1] = count * 4 + 1;
    indexData[count * 6 + 2] = count * 4 + 2;
    /**
    * Second triangle [3 -> 5]: /|
    *                          / |
    *                         /__|
    */
    indexData[count * 6 + 3] = count * 4 + 1;
    indexData[count * 6 + 4] = count * 4 + 2;
    indexData[count * 6 + 5] = count * 4 + 3;
}

/**
* @brief Creates the vertices for a Quad in an vertex buffer
* @param count - The place to write the data to in the index buffer
* @param vertexData - The vertex buffer to write to
* @param sprite - The Sprite the quad will have
* @param origin - The position the Quad will be placed in the world
* @param width - The width of the quad ; default = 1.0f
* @param height - The height of the quad ; default = 1.0f
*/
void Renderer::createQuadVertices(const Sprite* sprite, glm::vec2 origin) noexcept
{
    // default size is 16 x 16 px
    float width = (float)sprite->getWidth() / 16.0f;
    float height = (float)sprite->getHeight() / 16.0f;

    /**
    * @note - Fill the vertex data
    * @index 0 - the Top Left of the quad
    * @index 1 - the Top Right of the quad
    * @index 2 - the Bottom Left of the quad
    * @index 3 - the Bottom Right of the quad
    */
    vertexData[count * 4 + 0] = { {origin.x, origin.y + height}, {sprite->topLeft} };
    vertexData[count * 4 + 1] = { {origin.x + width, origin.y + height}, {sprite->topRight} };
    vertexData[count * 4 + 2] = { {origin.x, origin.y}, {sprite->bottomLeft} };
    vertexData[count * 4 + 3] = { {origin.x + width, origin.y}, {sprite->bottomRight} };
}

void Renderer::createQuadVertices(const Sprite* sprite, Rect* aabb) noexcept {
    /**
    * @note - Fill the vertex data
    * @index 0 - the Top Left of the quad
    * @index 1 - the Top Right of the quad
    * @index 2 - the Bottom Left of the quad
    * @index 3 - the Bottom Right of the quad
    */
    
    vertexData[count * 4 + 0] = { {aabb->pos.x - aabb->dim.x, aabb->pos.y + aabb->dim.y}, sprite->topLeft };
    vertexData[count * 4 + 1] = { {aabb->pos.x + aabb->dim.x, aabb->pos.y + aabb->dim.y}, sprite->topRight };
    vertexData[count * 4 + 2] = { {aabb->pos.x - aabb->dim.x, aabb->pos.y - aabb->dim.y}, sprite->bottomLeft };
    vertexData[count * 4 + 3] = { {aabb->pos.x + aabb->dim.x, aabb->pos.y - aabb->dim.y}, sprite->bottomRight };
}

Renderer::Renderer() : count(0u), vertexData(nullptr), indexData(nullptr)
{
    glfwSwapInterval(1); // 60 fps
    glEnable(GL_BLEND); // enable opacity for sprites
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glClearColor(142.f / 255.f, 144.f / 255.f, 253.f / 255.f, 1.0f);
    glClearColor(0, 0, 0, 1);

    sprites = new Sprite[20];

    // each number
    sprites[0] = createSprite(&mSpriteSheet, 0 * 7, 0, 7, 13);
    sprites[1] = createSprite(&mSpriteSheet, 1 * 7, 0, 7, 13);
    sprites[2] = createSprite(&mSpriteSheet, 2 * 7, 0, 7, 13);
    sprites[3] = createSprite(&mSpriteSheet, 3 * 7, 0, 7, 13);
    sprites[4] = createSprite(&mSpriteSheet, 4 * 7, 0, 7, 13);
    sprites[5] = createSprite(&mSpriteSheet, 5 * 7, 0, 7, 13);
    sprites[6] = createSprite(&mSpriteSheet, 6 * 7, 0, 7, 13);
    sprites[7] = createSprite(&mSpriteSheet, 7 * 7, 0, 7, 13);
    sprites[8] = createSprite(&mSpriteSheet, 8 * 7, 0, 7, 13);
    sprites[9] = createSprite(&mSpriteSheet, 9 * 7, 0, 7, 13);

    // color white
    sprites[10] = createSprite(&mSpriteSheet, 71, 1, 2, 2);
    sprites[11] = createSprite(&mSpriteSheet, 74, 1, 2, 2);
    sprites[12] = createSprite(&mSpriteSheet, 71, 4, 2, 2);
    sprites[13] = createSprite(&mSpriteSheet, 74, 4, 2, 2);
    sprites[14] = createSprite(&mSpriteSheet, 71, 7, 2, 2);
    sprites[15] = createSprite(&mSpriteSheet, 74, 7, 2, 2);
    sprites[16] = createSprite(&mSpriteSheet, 71, 10, 2, 2);
    sprites[17] = createSprite(&mSpriteSheet, 74, 10, 2, 2);
    
    // win / losses
    sprites[18] = createSprite(&mSpriteSheet, 11 * 7, 0, 7, 13); // W
    sprites[19] = createSprite(&mSpriteSheet, 12 * 7, 0, 7, 13); // L

    mShader = ShaderProgram("resources/shaders/textured/vertex.txt", "resources/shaders/textured/fragment.txt");
    
    vertexData = new Vertex[4LL * static_cast<long>(mMaxQuads)];
    indexData = new unsigned int[6LL * static_cast<long>(mMaxQuads)];

    // setup renderer
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mSpriteSheet.getID());

    /**
    * @note - vertex data
    * @size - equal to 10000 quads * 4 vertices * sizeof(Vertex)
    */
    glGenBuffers(1, &this->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4 * mMaxQuads, NULL, GL_DYNAMIC_DRAW);

    /**
    * @note - index data
    * @size - equal to 10000 quads * 6 indices * sizeof(unsigned int)
    */
    glGenBuffers(1, &this->indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6 * mMaxQuads, NULL, GL_DYNAMIC_DRAW);

    /**
    * @note - vertex attributes
    */
    glGenVertexArrays(1, &this->vertexAttributes);
    glBindVertexArray(this->vertexAttributes);

    /**
    * @note - setup the vertex attributes
    * @first - Attribute for the position of each vertex
    * @second - Atttribute for the texture coordinates of each vertex
    */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));

    mShader.use();
    mShader.setInt("sprite_sheet", 0);
}

Renderer::~Renderer() noexcept
{
    glDeleteVertexArrays(1, &this->vertexAttributes);
    glDeleteBuffers(1, &this->vertexBuffer);
    glDeleteBuffers(1, &this->indexBuffer);

    delete[] this->vertexData;
    delete[] this->indexData;

    delete[] this->sprites;
}

int Renderer::getSpriteCount(void) const noexcept {
    return this->spriteCount;
}

int Renderer::getSpriteID(const std::string& name) const noexcept {
    if (mSpriteNamesToIndex.find(name) != mSpriteNamesToIndex.end()) {
        return mSpriteNamesToIndex.at(name);
    }
    else {
        std::cerr << __FUNCTION__ << "Could not find sprite with name \"" << name << "\"\n";
        return 0;
    }
}

// resets the renderer for the next cycle
void Renderer::clear(void) noexcept
{
    this->count = 0;
}

// buffer a new sprite
void Renderer::buffer(glm::vec2 position, uint32_t spriteIndex) noexcept
{
    createQuadIndices();
    createQuadVertices(&sprites[spriteIndex], position);

    this->count++;
}

void Renderer::bufferQuad(Rect* aabb, unsigned int spriteIndex) noexcept 
{
    createQuadIndices();
    createQuadVertices(&sprites[spriteIndex], aabb);

    this->count++;
}

// draws everything that has been buffered
void Renderer::render(const Camera* camera) noexcept
{
    /** @note we just need projection here */
    mShader.use();
    mShader.setMat4("projection", camera->getProjection());

    glActiveTexture(GL_TEXTURE1);

    if (this->count > 0)
    {
        glBindVertexArray(this->vertexAttributes);

        glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4 * mMaxQuads, NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4 * this->count, vertexData);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * mMaxQuads, NULL, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * 6 * this->count, indexData);

        glDrawElements(GL_TRIANGLES, 6 * this->count, GL_UNSIGNED_INT, 0);
    }
}
