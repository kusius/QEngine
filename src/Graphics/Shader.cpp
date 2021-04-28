#include "Shader.h"

#include <iostream>
using namespace std;

void Shader::Compile(const GLchar *vertexSource, const GLchar *fragmentSource,
                     const GLchar *geometrySource)
{
    // compile the shaders
    GLuint vertex, fragment, geometry;

    // Vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Geometry
    if (geometrySource != nullptr)
    {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometrySource, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // Shader Program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, geometry);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");

    // shaders linked , they are no longer nessecary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometrySource != nullptr)
        glDeleteShader(geometry);
}
/*
Utility functions for setting various types of Uniforms
*/
void Shader::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y,
                         GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value,
                         GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z,
                         GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value,
                         GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y,
                value.z);
}
void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z,
                         GLfloat w, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value,
                         GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z,
                value.w);
}
void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix,
                        GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE,
                       glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(GLuint object, string type)
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 124, NULL, infoLog);
            cout << "[*] ERROR::SHADER: Compile-time error: Type: " << type
                 << "\n"
                 << infoLog
                 << "\n -- ------------------------------------------ --"
                 << endl;
        }
    }

    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            cout << "[*] ERROR::SHADER: Link-time error: Type: " << type << "\n"
                 << infoLog
                 << "\n -- ------------------------------------------ --"
                 << endl;
        }
    }
}

Shader &Shader::Use()
{
    glUseProgram(this->ID);
    return *this;
}