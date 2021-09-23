#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

bool load_shader(string filepath, vector<char*> &shaders)
{
    ifstream is;

    is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        is.open(filepath);

        stringstream ss;

        ss << is.rdbuf();

        is.close();

        auto code = ss.str();

        char *shaderCode = new char[code.size() + 1];

        strcpy(shaderCode, code.c_str());

        shaderCode[code.size()] = 0;

        shaders.push_back(shaderCode);
    }
    catch (ifstream::failure& e) { 

        cout << "Error Read Shader" << endl;
        return false;
    }
    return true;
}

GLuint compileShaders(const char *code, GLenum type)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        char message[1024];
        glGetShaderInfoLog(shader, 1024, NULL, message);
        cout << "Error Compile Shader: " << message << endl;

        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint linkProgram(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint linkStatus;

    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    {
        char message[1024];
        glGetProgramInfoLog(program, 1024, NULL, message);
        cout << "Error Link Program: " << message << endl;

        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

GLuint linkProgram(GLuint vertex_shader, GLuint geometry_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, geometry_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint linkStatus;

    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    {
        char message[1024];
        glGetProgramInfoLog(program, 1024, NULL, message);
        cout << "Error Link Program: " << message << endl;
        
        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

void setBool(GLuint program, const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}

void setInt(GLuint program, const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void setFloat(GLuint program, const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void setVec2(GLuint program, const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void setVec2(GLuint program, const std::string &name, float x, float y)
{
    glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
}

void setVec3(GLuint program, const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void setVec3(GLuint program, const std::string &name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
}

void setVec4(GLuint program, const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void setVec4(GLuint program, const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
}

void setMat2(GLuint program, const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void setMat3(GLuint program, const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void setMat4(GLuint program, const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}