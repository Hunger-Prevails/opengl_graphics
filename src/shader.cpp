#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <map>
#include <string>
#include <cassert>

#include "shader.h"

using namespace std;

void Shader::load_shader(string filepath, GLenum type) {

	auto segment = load_shader(filepath);

	shaders.push_back(compile_shader(segment, type));
}

char* Shader::load_shader(string filepath) {

	ifstream is;

    is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        is.open(filepath);

        stringstream ss;

        ss << is.rdbuf();

        is.close();

        auto source = ss.str();

        char *segment = new char[source.size() + 1];

        strcpy(segment, source.c_str());

        segment[source.size()] = 0;

        return segment;
    }
    catch (ifstream::failure& e) {

        cout << "fails to read shader source file" << endl;
        exit(0);
    }
}

unsigned int Shader::compile_shader(char *segment, GLenum type)
{
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &segment, nullptr);
    glCompileShader(shader);

    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        char message[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, message);
        cout << "fails to compile shader source: " << message << endl;

        glDeleteShader(shader);
        exit(0);
    }
    return shader;
}

void Shader::link()
{
    this->program = glCreateProgram();

    for (auto shader: shaders) glAttachShader(this->program, shader);

    glLinkProgram(this->program);

    int linkStatus;
    glGetProgramiv(this->program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    {
        char message[1024];
        glGetProgramInfoLog(this->program, 1024, nullptr, message);
        cout << "fails to link shader program: " << message << endl;

        for (auto shader: shaders) glDetachShader(this->program, shader);

        glDeleteProgram(this->program);
        exit(0);
    }
}

void Shader::use()
{
	glUseProgram(this->program);
}

int Shader::get_attr_location(string attr_name)
{
	return glGetAttribLocation(this->program, attr_name.c_str());
}

int Shader::get_uniform_location(string uniform_name)
{
	return glGetUniformLocation(this->program, uniform_name.c_str());
}

void Shader::setBool(const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(this->program, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(this->program, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(glGetUniformLocation(this->program, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y)
{
    glUniform2f(glGetUniformLocation(this->program, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(this->program, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(this->program, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(glGetUniformLocation(this->program, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(this->program, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
