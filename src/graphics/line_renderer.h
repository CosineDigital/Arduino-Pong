#pragma once

#include <glm/glm.hpp>

#include "shader_program.h"
#include "../core/camera.h"

class LineRenderer final {

public:
    LineRenderer();
    ~LineRenderer() noexcept;

    void buffer(glm::vec2 start, glm::vec2 end) noexcept;

    void clear() noexcept;

    void render(const Camera* camera) noexcept;


private:
    ShaderProgram mShader;

    unsigned int count;
    unsigned int vertexBuffer;
    unsigned int vertexAttributes;

    glm::vec2* lineData{ nullptr };

};