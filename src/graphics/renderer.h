#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include "../graphics/shader_program.h"
#include "../graphics/sprite_sheet.h"
#include "../graphics/sprite.h"
#include "../graphics/vertex.h"
#include "../core/camera.h"
#include "C:\C++ Libraries\glad\include\glad\glad.h"
#include "C:\C++ Libraries\glfw-3.3.2.bin.WIN64\glfw-3.3.2.bin.WIN64\include\GLFW\glfw3.h"
#include "C:\C++ Libraries\glad\include\KHR\khrplatform.h"
#include "C:\C++ Libraries\glm-0.9.9.8\glm\glm.hpp"

class Renderer
{
public:
    friend class Editor;

    explicit Renderer();
    ~Renderer() noexcept;

    void buffer(glm::vec2 position, uint32_t spriteIndex) noexcept;
    void bufferQuad(glm::vec2 pos, glm::vec2 dim) noexcept;

    void clear() noexcept;

    void render(const Camera* camera) noexcept;

    int getSpriteCount(void) const noexcept;

    int getSpriteID(const std::string& name) const noexcept;

private:

    void createQuadIndices() noexcept;
    void createQuadVertices(const Sprite* sprite, glm::vec2 origin) noexcept;

    void createQuadVertices(glm::vec2 pos, glm::vec2 dim) noexcept;

private:

    std::map<std::string, int> mSpriteNamesToIndex;

    GLFWwindow* mWindow;
    ShaderProgram mShader;

    // TODO: make sure this is dynamic, in case we go over this limit
    int mMaxQuads = 100;

    int count;

    GLuint vertexAttributes;

    GLuint vertexBuffer;
    GLuint indexBuffer;

    Vertex* vertexData;
    GLuint* indexData;

    // TODO fix
    SpriteSheet mSpriteSheet = loadSpriteSheet("resources/sprites/sprites.png");

    Sprite* sprites;
    int spriteCount;
};

#endif // !RENDERER_H_

