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
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "tex_manager.h"

using namespace std;

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

shared_ptr<Model> backpack;

void init()
{
    glClearColor(0.2f, 0.2f, 0.2f, 0);
    glEnable(GL_DEPTH_TEST);
    glutSetCursor(GLUT_CURSOR_NONE);

    auto shader = make_shared<Shader>();

    shader->load_shader("shaders/main.vs", GL_VERTEX_SHADER);
    shader->load_shader("shaders/main.fs", GL_FRAGMENT_SHADER);

    shader->link();

    backpack = make_shared<Model>("res/backpack/backpack.obj", move(shader), true);

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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 perspect = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    auto view_x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    auto view_y = sin(glm::radians(pitch));
    auto view_z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cam_front = glm::normalize(glm::vec3(view_x, view_y, view_z));

    glm::mat4 view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_oben);

    backpack->get_shader()->use();

    backpack->get_shader()->setMat4("uModel", model);
    backpack->get_shader()->setMat4("uView", view);
    backpack->get_shader()->setMat4("uPerspect", perspect);
    backpack->get_shader()->setVec3("uCamPos", cam_pos);

    backpack->get_shader()->setVec3("uLight.position", glm::vec3(1.2f, 1.0f, 2.0f));
    backpack->get_shader()->setVec3("uLight.direction", glm::vec3(-1.2f, -1.0f, -2.0f));
    backpack->get_shader()->setVec3("uLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    backpack->get_shader()->setVec3("uLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    backpack->get_shader()->setVec3("uLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    backpack->get_shader()->setFloat("uLight.atten_a", 0.0f);
    backpack->get_shader()->setFloat("uLight.atten_b", 0.0f);
    backpack->get_shader()->setFloat("uLight.cone_a", glm::cos(glm::radians(10.0f)));
    backpack->get_shader()->setFloat("uLight.cone_b", glm::cos(glm::radians(15.0f)));

    backpack->render();

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

    if (key == 'w')
        cam_pos += delta_value * cam_front * 5.0f;
    if (key == 's')
        cam_pos -= delta_value * cam_front * 5.0f;
    if (key == 'a')
        cam_pos += delta_value * cam_links * 5.0f;
    if (key == 'd')
        cam_pos -= delta_value * cam_links * 5.0f;
}

void mouse(int button, int state, int xpos, int ypos)
{
    if (button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN)
    {

        last_mouse_x = xpos;
        last_mouse_y = ypos;

        drags = true;
    }
    else if (button == GLUT_RIGHT_BUTTON and state == GLUT_UP)
    {

        drags = false;
    }
}

void motion(int xpos, int ypos)
{
    if (!drags)
        return;

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