#include "shader_program.h"

/**
* @brief constructor for a shader program
* @param vertexPath - the full path to the vertex shader source relative to the .exe
* @param fragmentPath - the full path to the fragment shader source relative to the .exe
*/
ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    this->programID = glCreateProgram();
    // create, attach, and destroy vertex shader
    Shader vertexShader(vertexPath, GL_VERTEX_SHADER);
    glAttachShader(this->programID, vertexShader.getID());
    glDeleteShader(vertexShader.getID());
    // create, attach, and destroy fragment shader
    Shader fragmentShader(fragmentPath, GL_FRAGMENT_SHADER);
    glAttachShader(this->programID, fragmentShader.getID());
    glDeleteShader(fragmentShader.getID());
    
    glLinkProgram(this->programID);
}

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    this->programID = glCreateProgram();
    // create, attach, and destroy vertex shader
    Shader vertexShader(vertexPath, GL_VERTEX_SHADER);
    glAttachShader(this->programID, vertexShader.getID());
    glDeleteShader(vertexShader.getID());
    // create, attach, and destroy geometry shader
    Shader geometryShader(geometryPath, GL_GEOMETRY_SHADER);
    glAttachShader(this->programID, geometryShader.getID());
    glDeleteShader(geometryShader.getID());
    // create, attach, and destroy fragment shader
    Shader fragmentShader(fragmentPath, GL_FRAGMENT_SHADER);
    glAttachShader(this->programID, fragmentShader.getID());
    glDeleteShader(fragmentShader.getID());

    glLinkProgram(this->programID);
}

/**
* @brief Set as the default rendering shader program
*/
void ShaderProgram::use(void) const
{
    glUseProgram(this->programID);
}

/***
* @return This ShaderProgram's internal OpenGL ID
*/
const unsigned int& ShaderProgram::getID(void) const noexcept
{
    return this->programID;
}

/**
* @brief Returns the ID of a uniform
* @param uniformName - The uniform needed
* @return - The ID of the uniform from either the vertex or fragment shader
*/
GLint ShaderProgram::getUniformLocation(const char* uniformName) const
{
    if (this->uniformLocations.find(uniformName) != this->uniformLocations.end())
    {
        return this->uniformLocations.at(uniformName);
    }
    else
    {
        // place in memory for better performance
        return (this->uniformLocations[uniformName] = glGetUniformLocation(this->programID, uniformName));
    }
}

// ==============================================
// [Section] Uniform Setters
// ==============================================

void ShaderProgram::setBool(const char* uniformName, bool value) const
{
    glUniform1i(this->getUniformLocation(uniformName), value);
}

void ShaderProgram::setInt(const char* uniformName, int value) const
{
    glUniform1i(this->getUniformLocation(uniformName), value);
}

void ShaderProgram::setFloat(const char* uniformName, float value) const
{
    glUniform1f(this->getUniformLocation(uniformName), value);
}

void ShaderProgram::setVec2(const char* uniformName, const glm::vec2& value) const
{
    glUniform2fv(this->getUniformLocation(uniformName), 1, &value[0]);
}

void ShaderProgram::setVec2(const char* uniformName, float x, float y) const
{
    glUniform2f(this->getUniformLocation(uniformName), x, y);
}

void ShaderProgram::setVec3(const char* uniformName, const glm::vec3& value) const
{
    glUniform3fv(this->getUniformLocation(uniformName), 1, &value[0]);
}

void ShaderProgram::setVec3(const char* uniformName, float x, float y, float z) const
{
    glUniform3f(this->getUniformLocation(uniformName), x, y, z);
}

void ShaderProgram::setVec4(const char* uniformName, const glm::vec4 &value) const
{
    glUniform4fv(this->getUniformLocation(uniformName), 1, &value[0]);
}

void ShaderProgram::setVec4(const char* uniformName, float x, float y, float z, float w) const
{
    glUniform4f(this->getUniformLocation(uniformName), x, y, z, w);
}

void ShaderProgram::setMat2(const char* uniformName, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(this->getUniformLocation(uniformName), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat3(const char* uniformName, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(this->getUniformLocation(uniformName), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const char* uniformName, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(this->getUniformLocation(uniformName), 1, GL_FALSE, &mat[0][0]);
}
