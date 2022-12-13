#include "line_renderer.h"

#define MAX_LINES 1000

LineRenderer::LineRenderer() : count(0)
{
	
	mShader = ShaderProgram("resources/shaders/line/vertex.txt",
		"resources/shaders/line/fragment.txt",
		"resources/shaders/line/geometry.txt");

	// up to 10,000 lines
	lineData = new glm::vec2[MAX_LINES];

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAX_LINES, NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &vertexAttributes);
	glBindVertexArray(vertexAttributes);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
}

LineRenderer::~LineRenderer() noexcept {

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &vertexAttributes);
	delete[] lineData;
}

void LineRenderer::clear() noexcept {
	count = 0;
}

void LineRenderer::buffer(glm::vec2 start, glm::vec2 end) noexcept {

	lineData[count++] = start;
	lineData[count++] = end;
}

void LineRenderer::render(const Camera* camera) noexcept {

	mShader.use();
	mShader.setMat4("projection", camera->getProjection());

	if (count > 0)
	{
		glBindVertexArray(vertexAttributes);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAX_LINES, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * this->count, lineData);
		
		glDrawArrays(GL_LINES, 0, this->count);
	}
}