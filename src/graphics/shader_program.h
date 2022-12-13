#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <string>

#include "C:\C++ Libraries\glad\include\glad\glad.h"
#include "C:\C++ Libraries\glfw-3.3.2.bin.WIN64\glfw-3.3.2.bin.WIN64\include\GLFW\glfw3.h"
#include "C:\C++ Libraries\glad\include\KHR\khrplatform.h"
#include "C:\C++ Libraries\glm-0.9.9.8\glm\glm.hpp"

#include "shader.h"

class ShaderProgram
{
public:
    ShaderProgram() = default;

    // @constructor a regular shader program
    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    // @constructor a shader program with a geometry shader
    ShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath);


    void use(void) const;
    const unsigned int& getID(void) const noexcept;

    void setBool (const char* uniformName, bool  value) const;
    void setInt  (const char* uniformName, int   value) const;
    void setFloat(const char* uniformName, float value) const;

    void setVec2(const char* uniformName, const glm::vec2& value)             const;
    void setVec2(const char* uniformName, float x, float y)                   const;
    void setVec3(const char* uniformName, const glm::vec3& value)             const;
    void setVec3(const char* uniformName, float x, float y, float z)          const;
    void setVec4(const char* uniformName, const glm::vec4 &value)             const;
    void setVec4(const char* uniformName, float x, float y, float z, float w) const;

    void setMat2(const char* uniformName, const glm::mat2 &mat) const;
    void setMat3(const char* uniformName, const glm::mat3 &mat) const;
    void setMat4(const char* uniformName, const glm::mat4 &mat) const;

private:
    mutable std::unordered_map<const char*, int> uniformLocations;
    int getUniformLocation(const char* uniformName) const;
public:
    unsigned int programID{ 0 };
};

#endif // SHADER_PROGRAM_H_
