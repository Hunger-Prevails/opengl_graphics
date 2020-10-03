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
GLuint loadDataInBuffers()
{
    GLfloat vertices[] = {// triangle vertex coordinates
                          -0.5, -0.5, 0,
                          0.5, -0.5, 0,
                          0, 0.5, 0};

    GLuint vboId;

    // allocate buffer sapce and pass data to it
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // unbind the active buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboId;
}

// Initialize and put everything together
void init()
{
    // clear the framebuffer each frame with black color
    glClearColor(0.3, 0.5f, 0, 0);

    GLuint vboId = loadDataInBuffers();

    vector<char*> shaders;

    if (!load_shader("../shaders/vs.glsl", shaders)) exit(0);
    if (!load_shader("../shaders/fs.glsl", shaders)) exit(0);

    GLuint vShaderId = compileShaders(shaders[0], GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(shaders[1], GL_FRAGMENT_SHADER);

    GLuint programId = linkProgram(vShaderId, fShaderId);

    // Get the 'pos' variable location inside this program
    GLuint posAttributePosition = glGetAttribLocation(programId, "pos");

    GLuint vaoId;
    glGenVertexArrays(1, &vaoId); // Generate VAO  (Vertex Array Object)

    // Bind it so that rest of vao operations affect this vao
    glBindVertexArray(vaoId);

    // buffer from which 'pos' will receive its data and the format of that data
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(posAttributePosition, 3, GL_FLOAT, false, 0, 0);

    // Enable this attribute array linked to 'pos'
    glEnableVertexAttribArray(posAttributePosition);

    // Use this program for rendering.
    glUseProgram(programId);
}

// Function that does the drawing
// glut calls this function whenever it needs to redraw
void display()
{
    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT);

    // draw triangles starting from index 0 and
    // using 3 indices
    glDrawArrays(GL_TRIANGLES, 0, 3);

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
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Triangle Using OpenGL");
    glewInit();
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}