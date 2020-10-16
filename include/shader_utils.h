#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

unsigned long getFileLength(ifstream& is)
{
    if(!is.good()) return 0;
    
    unsigned long pos = is.tellg();
    is.seekg(0,ios::end);
    unsigned long len = is.tellg();
    is.seekg(ios::beg);
    
    return len;
}

bool load_shader(string filename, vector<char*> &shaders)
{
    ifstream is;
    is.open(filename, ios::in);  // opens as ASCII!
    if(!is) return false;

    unsigned long len = getFileLength(is);

    char *str = new char[len + 1];

    unsigned int i = 0;
    while (!is.eof())
    {
        char ch = is.get();
        if (int(ch) == -1) break;
        str[i] = ch;  // get character from is.
        i++;
    }
    str[i] = 0;  // 0-terminate it at the correct positions

    shaders.push_back(str);

    is.close();

    return true;
}

// Compile and create shader object and returns its id
GLuint compileShaders(char *shaderSource, GLenum type)
{
    GLuint shaderId = glCreateShader(type);

    // Error: Cannot create shader object
    if (shaderId == 0)
    {
        cout << "Error creating shaders!";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderSource, NULL);
    glCompileShader(shaderId); // compile the shader object

    GLint compileStatus;

    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    // If compilation fails
    if (!compileStatus)
    {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

// Creates a program containing vertex and fragment shader
// links it and returns its program
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint program = glCreateProgram(); // create a program

    if (program == 0)
    {
        cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(program, vertexShaderId);
    glAttachShader(program, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(program);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    { // If the linking failed
        cout << "Error Linking program";
        glDetachShader(program, vertexShaderId);
        glDetachShader(program, fragmentShaderId);
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