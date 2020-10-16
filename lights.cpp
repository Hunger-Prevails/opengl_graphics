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
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tex_manager.h"
#include "shader_utils.h"
#include "vertices.h"

using namespace std;

GLuint loadArrayBuffer(vector<float> attributes)
{
    GLuint array_buffer;

    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(float), attributes.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return array_buffer;
}

extern float vertex_data[];

TexManager *tex_manager;

GLuint vao;
GLuint program;

glm::vec3 cam_pos;
glm::vec3 cam_front;
glm::vec3 cam_oben;

float time_value;
float last_value;

int last_mouse_x;
int last_mouse_y;
bool drags;

float pitch;
float yaw;

void init()
{
    glClearColor(0.2f, 0.2f, 0.2f, 0);
    glEnable(GL_DEPTH_TEST);
    glutSetCursor(GLUT_CURSOR_NONE);

    vector<float> data;
    data.assign(vertex_data, vertex_data + 36 * 8);
    GLuint array_buffer = loadArrayBuffer(data);

    vector<char*> shaders;

    if (!load_shader("../shaders/lights.vs", shaders)) exit(0);
    if (!load_shader("../shaders/lights.fs", shaders)) exit(0);

    GLuint vShaderId = compileShaders(shaders[0], GL_VERTEX_SHADER);
    GLuint fShaderId = compileShaders(shaders[1], GL_FRAGMENT_SHADER);

    program = linkProgram(vShaderId, fShaderId);

    GLuint position_attrib = glGetAttribLocation(program, "aPosition");
    GLuint normal_attrib = glGetAttribLocation(program, "aNormal");
    GLuint tex_coord_attrib = glGetAttribLocation(program, "aTexCoord");

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, false, 8 * sizeof(float), (void *)0);
    glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, false, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(tex_coord_attrib, 2, GL_FLOAT, false, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glEnableVertexAttribArray(position_attrib);
    glEnableVertexAttribArray(normal_attrib);
    glEnableVertexAttribArray(tex_coord_attrib);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    tex_manager = new TexManager();
    tex_manager->load_texture("../res/diffuse.png", "uDiffuse");
    tex_manager->load_texture("../res/specular.png", "uSpecular");

    cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
    cam_oben = glm::vec3(0.0f, 1.0f, 0.0f);

    drags = false;

    pitch = 0.0f;
    yaw = -90.0f;
}

void display()
{
    last_value = time_value;

    auto millisec = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

    time_value = (millisec % 1000000) / 1000.0;

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    
    glBindVertexArray(vao);

    tex_manager->upload(program, "uDiffuse");
    tex_manager->upload(program, "uSpecular");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 perspect = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    auto view_x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    auto view_y = sin(glm::radians(pitch));
    auto view_z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cam_front = glm::normalize(glm::vec3(view_x, view_y, view_z));

    glm::mat4 view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_oben);

    auto model_uniform = glGetUniformLocation(program, "uModel");
    auto view_uniform = glGetUniformLocation(program, "uView");
    auto perspect_uniform = glGetUniformLocation(program, "uPerspect");

    glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(perspect_uniform, 1, GL_FALSE, glm::value_ptr(perspect));

    auto shininess_uniform = glGetUniformLocation(program, "uShininess");
    glUniform1f(shininess_uniform, 32.0);

    auto cam_pos_uniform = glGetUniformLocation(program, "uCamPos");
    glUniform3fv(cam_pos_uniform, 1, glm::value_ptr(cam_pos));

    auto light_pos_uniform = glGetUniformLocation(program, "uLight.position");
    auto light_ambient_uniform = glGetUniformLocation(program, "uLight.ambient");
    auto light_diffuse_uniform = glGetUniformLocation(program, "uLight.diffuse");
    auto light_specular_uniform = glGetUniformLocation(program, "uLight.specular");

    glUniform3f(light_pos_uniform, 1.2f, 1.0f, 2.0f);
    glUniform3f(light_ambient_uniform, 0.2f, 0.2f, 0.2f);
    glUniform3f(light_diffuse_uniform, 0.5f, 0.5f, 0.5f);
    glUniform3f(light_specular_uniform, 1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glutSwapBuffers();
}

void timer(int value)
{
    glutTimerFunc(16, timer, 0);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int xmouse, int ymouse)
{
    float delta_value = time_value - last_value;

    auto cam_links = glm::normalize(glm::cross(cam_oben, cam_front));

    if (key == 'w') cam_pos += delta_value * cam_front * 2.0f;
    if (key == 's') cam_pos -= delta_value * cam_front * 2.0f;
    if (key == 'a') cam_pos += delta_value * cam_links * 2.0f;
    if (key == 'd') cam_pos -= delta_value * cam_links * 2.0f;
}

void mouse(int button, int state, int xpos, int ypos)
{
    if (button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN) {
        
        last_mouse_x = xpos;
        last_mouse_y = ypos;

        drags = true;

    } else if (button == GLUT_RIGHT_BUTTON and state == GLUT_UP) {

        drags = false;
    }
}

void motion(int xpos, int ypos)
{
    if (!drags) return;

    float xoffset = xpos - last_mouse_x;
    float yoffset = last_mouse_y - ypos;
    last_mouse_x = xpos;
    last_mouse_y = ypos;

    pitch += yoffset * 0.1f;
    yaw += xoffset * 0.1f;

    pitch = max(min(pitch, 60.0f), -60.0f);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Triangle Using OpenGL");
    glewInit();
    init();
    glutTimerFunc(16, timer, 0);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}