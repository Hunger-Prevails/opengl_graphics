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
// links it and returns its ID
GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // create a program

    if (programId == 0)
    {
        cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    { // If the linking failed
        cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}
