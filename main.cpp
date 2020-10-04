#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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
    str[i] = 0;  // 0-terminate it at the correct position

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

// Load data in VBO (Vertex Buffer Object) and return the vbo's id
GLuint loadArrayBuffer()
{
    GLfloat vertices[] = {-0.5, -0.5, 0, 0.5, -0.5, 0, -0.5, 0.5, 0, 0.5, 0.5, 0};

    GLuint array_buffer;

    // allocate buffer sapce and pass data to it
    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // unbind the active buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return array_buffer;
}

GLuint loadElementBuffer()
{
    GLuint indices[] = {0, 1, 2, 1, 3, 2};

    GLuint element_buffer;

    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return element_buffer;
}

GLuint vertex_array;
GLuint programId;

// Initialize and put everything together
void init()
{
    // clear the framebuffer each frame with black color
    glClearColor(0.3, 0.5f, 0, 0);

    GLuint array_buffer = loadArrayBuffer();
    GLuint element_buffer = loadElementBuffer();

    vector<char*> shaders;

    if (!load_shader("../shaders/vs.glsl", shaders)) exit(0);
    if (!load_shader("../shaders/fs.glsl", shaders)) exit(0);

    GLuint vShaderId = compileShaders(shaders[0], GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(shaders[1], GL_FRAGMENT_SHADER);

    programId = linkProgram(vShaderId, fShaderId);

    // Get the 'pos' variable location inside this program
    GLuint posAttributePosition = glGetAttribLocation(programId, "pos");

    glGenVertexArrays(1, &vertex_array); // Generate VAO  (Vertex Array Object)

    // Bind it so that rest of vao operations affect this vao
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

    glVertexAttribPointer(posAttributePosition, 3, GL_FLOAT, false, 0, 0);

    // Enable this attribute array linked to 'pos'
    glEnableVertexAttribArray(posAttributePosition);

    // unbind objects
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Function that does the drawing
// glut calls this function whenever it needs to redraw
void display()
{
    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT);

    // use this program for rendering.
    glUseProgram(programId);
    // use this vertex array object for attribute arrangement
    glBindVertexArray(vertex_array);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // swap the buffers and hence show the buffers
    // content to the screen
    glutSwapBuffers();
}

// main function
// sets up window to which we'll draw
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(1600, 900);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Triangle Using OpenGL");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}