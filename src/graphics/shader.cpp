#include "shader.h"

Shader::Shader(const char* sourcePath, GLenum type)
{
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
       file.open(sourcePath);
       std::stringstream stream;
       stream << file.rdbuf();
       file.close();
       code = stream.str();
    }
    catch (std::ifstream::failure f)
    {
        std::cout << "Could not read shader file @ " << sourcePath << '\n';
    }

    const char* shaderCode = code.c_str();
    this->shaderID = glCreateShader(type);
    glShaderSource(this->shaderID, 1, &shaderCode, 0);
    glCompileShader(this->shaderID);
    this->checkErrors(this->shaderID, type);
}

GLuint Shader::getID(void) const
{
    return this->shaderID;
}

void Shader::checkErrors(GLuint shader, GLenum type)
{
    GLint noErrors;
    GLchar log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &noErrors);
    glGetShaderInfoLog(shader, 1024, 0, log);
    if(!noErrors)
    {
       std::cout << ((type == GL_VERTEX_SHADER)
                    ? "Error compiling vertex shader\n"
                    : "Error compiling fragment shader\n")
                    << log << '\n';
    }
}
