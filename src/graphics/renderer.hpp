#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <KHR/khrplatform.h>
#include <glm/glm.hpp>

#include "../graphics/shader_program.hpp"
#include "../graphics/sprite_sheet.hpp"
#include "../graphics/sprite.hpp"
#include "../graphics/vertex.hpp"
#include "../core/camera.hpp"
#include "../core/rect.hpp"

class Renderer
{
public:
    friend class Editor;

    explicit Renderer();
    ~Renderer() noexcept;

    void buffer(glm::vec2 position, uint32_t spriteIndex) noexcept;
    void bufferQuad(Rect* aabb, unsigned int spriteIndex) noexcept;

    void clear() noexcept;

    void render(const Camera* camera) noexcept;

    int getSpriteCount(void) const noexcept;

    int getSpriteID(const std::string& name) const noexcept;

private:

    void createQuadIndices() noexcept;
    void createQuadVertices(const Sprite* sprite, glm::vec2 origin) noexcept;

    void createQuadVertices(const Sprite* sprite, Rect* aabb) noexcept;

private:

    std::map<std::string, int> mSpriteNamesToIndex;

    GLFWwindow* mWindow;
    ShaderProgram mShader;

    // TODO: make sure this is dynamic, in case we go over this limit
    int mMaxQuads = 15;

    int count;

    GLuint vertexAttributes;

    GLuint vertexBuffer;
    GLuint indexBuffer;

    Vertex* vertexData;
    GLuint* indexData;

    // TODO fix
    SpriteSheet mSpriteSheet = loadSpriteSheet("resources/textures/.png");

    Sprite* sprites;
    int spriteCount;
};

#endif // !RENDERER_H_

