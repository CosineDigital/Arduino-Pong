#ifndef SHADER_H_
#define SHADER_H_

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
    explicit Shader(const char* sourcePath, GLenum type);

    GLuint getID(void) const;
private:
    GLuint shaderID;
    void checkErrors(GLuint shader, GLenum type);
};

#endif // SHADER_H_
