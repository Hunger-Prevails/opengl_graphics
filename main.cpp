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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader_utils.h"

using namespace std;

// Load data in VBO (Vertex Buffer Object) and return the vbo's id
GLuint loadArrayBuffer(vector<float> attributes)
{
    GLuint array_buffer;

    // allocate buffer sapce and pass data to it
    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(float), attributes.data(), GL_STATIC_DRAW);

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

GLuint load_texture(string image_file)
{
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height;
    auto data = stbi_load(image_file.c_str(), &width, &height, 0, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint vertex_array;
GLuint programId;

// Initialize and put everything together
void init()
{
    GLuint texture = load_texture("../res/container.png");

    // clear the framebuffer each frame with black color
    glClearColor(0.2f, 0.2f, 0.2f, 0);

    float vertex_position[] = {-0.5, -0.5, 0, 0.5, -0.5, 0, -0.5, 0.5, 0, 0.5, 0.5, 0};

    vector<float> positions;

    positions.assign(vertex_position, vertex_position + 12);

    GLuint position_buffer = loadArrayBuffer(positions);

    float vertex_color[] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};

    vector<float> colors;

    colors.assign(vertex_color, vertex_color + 12);

    GLuint color_buffer = loadArrayBuffer(colors);

    GLuint element_buffer = loadElementBuffer();

    vector<char*> shaders;

    if (!load_shader("../shaders/vs.glsl", shaders)) exit(0);
    if (!load_shader("../shaders/fs.glsl", shaders)) exit(0);

    GLuint vShaderId = compileShaders(shaders[0], GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(shaders[1], GL_FRAGMENT_SHADER);

    programId = linkProgram(vShaderId, fShaderId);

    // Get the 'pos' variable location inside this program
    GLuint position_attrib = glGetAttribLocation(programId, "aPosition");
    GLuint color_attrib = glGetAttribLocation(programId, "aColor");

    glGenVertexArrays(1, &vertex_array); // Generate VAO  (Vertex Array Object)

    // Bind it so that rest of vao operations affect this vao
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, false, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glVertexAttribPointer(color_attrib, 3, GL_FLOAT, false, 0, 0);

    // Enable this attribute array linked to 'pos'
    glEnableVertexAttribArray(position_attrib);
    glEnableVertexAttribArray(color_attrib);

    // unbind objects
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    int volume_uniform = glGetUniformLocation(programId, "uVolume");
    auto time_value = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    glUniform1f(volume_uniform, sin(time_value * 3.1416 / 2000.0f) / 2.0f + 0.5f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // swap the buffers and hence show the buffers
    // content to the screen
    glutSwapBuffers();
}

void timer( int value )
{
    glutTimerFunc(16, timer, 0);
    glutPostRedisplay();
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
    glutTimerFunc(16, timer, 0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}