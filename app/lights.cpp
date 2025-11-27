#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <chrono>
#include <cmath>
#include <memory>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data.h"
#include "shader.h"
#include "tex_manager.h"
#include "frame_buffer.h"
#include "attr_array.h"

using namespace std;

int screen_w = 1200;
int screen_h = 800;

shared_ptr<FrameBuffer> frame_buffer;
shared_ptr<TexManager> tex_manager;

shared_ptr<AttrArray> cube_attr;
shared_ptr<AttrArray> screen_attr;
shared_ptr<AttrArray> skybox_attr;

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
    glutSetCursor(GLUT_CURSOR_NONE);

    tex_manager = make_shared<TexManager>();

    tex_manager->load_texture("res/diffuse.png", "uDiffuse");
    tex_manager->load_texture("res/specular.png", "uSpecular");
    tex_manager->load_skybox("res/skybox", "uSkybox");

    frame_buffer = make_shared<FrameBuffer>(screen_w, screen_h);

    tex_manager->add_texture("screen_frame", frame_buffer->get_buffer(), "uScreen");

    vector<float> cube_data;
    cube_data.assign(cube_vertices, cube_vertices + 288);

    vector<pair<string, size_t>> cube_attr_sizes;

    cube_attr_sizes.push_back(make_pair("aPos", 3));
    cube_attr_sizes.push_back(make_pair("aNormal", 3));
    cube_attr_sizes.push_back(make_pair("aTexCoord", 2));

    auto cube_shader = make_shared<Shader>();

    cube_shader->load_shader("shaders/lights.vs", GL_VERTEX_SHADER);
    cube_shader->load_shader("shaders/lights.gs", GL_GEOMETRY_SHADER);
    cube_shader->load_shader("shaders/lights.fs", GL_FRAGMENT_SHADER);

    cube_shader->link();

    cube_attr = make_shared<AttrArray>(cube_data, cube_attr_sizes, move(cube_shader));
    cube_attr->add_tex_path("res/diffuse.png");
    cube_attr->add_tex_path("res/specular.png");
    cube_attr->add_tex_path("res/skybox");

    auto model_a = glm::mat4(1.0f);
    auto model_b = glm::translate(glm::mat4(1.0f), glm::vec3(-1.2f, -1.2f, -2.0f));
    model_b = glm::scale(model_b, glm::vec3(0.8f));
    auto model_c = glm::translate(glm::mat4(1.0f), glm::vec3(-2.4f, -1.8f, -4.0f));
    model_c = glm::scale(model_c, glm::vec3(1.6f));

    vector<glm::mat4> models;

    models.push_back(model_a);
    models.push_back(model_b);
    models.push_back(model_c);

    cube_attr->set_inst_mat4(models, "iModel");

    vector<float> skybox_data;
    skybox_data.assign(skybox_vertices, skybox_vertices + 108);

    vector<pair<string, size_t>> skybox_attr_sizes;

    skybox_attr_sizes.push_back(make_pair("aPos", 3));

    auto skybox_shader = make_shared<Shader>();

    skybox_shader->load_shader("shaders/skybox.vs", GL_VERTEX_SHADER);
    skybox_shader->load_shader("shaders/skybox.fs", GL_FRAGMENT_SHADER);

    skybox_shader->link();

    skybox_attr = make_shared<AttrArray>(skybox_data, skybox_attr_sizes, move(skybox_shader));
    skybox_attr->add_tex_path("res/skybox");

    vector<float> screen_data;
    screen_data.assign(screen_vertices, screen_vertices + 12);

    vector<pair<string, size_t>> screen_attr_sizes;

    screen_attr_sizes.push_back(make_pair("aPos", 2));

    auto screen_shader = make_shared<Shader>();

    screen_shader->load_shader("shaders/screen.vs", GL_VERTEX_SHADER);
    screen_shader->load_shader("shaders/screen.fs", GL_FRAGMENT_SHADER);

    screen_shader->link();

    screen_attr = make_shared<AttrArray>(screen_data, screen_attr_sizes, move(screen_shader));
    screen_attr->add_tex_path("screen_frame");

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

    auto periodic = sin(time_value * 3.1416 / 2.0) / 2.0f + 0.5f;

    frame_buffer->bind();

    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto perspect = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    auto view_x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    auto view_y = sin(glm::radians(pitch));
    auto view_z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cam_front = glm::normalize(glm::vec3(view_x, view_y, view_z));

    auto view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_oben);

    cube_attr->get_shader()->use();

    cube_attr->get_shader()->setMat4("uView", view);
    cube_attr->get_shader()->setMat4("uPerspect", perspect);
    cube_attr->get_shader()->setVec3("uCamPos", cam_pos);

    cube_attr->get_shader()->setFloat("uShininess", 64.0);
    cube_attr->get_shader()->setFloat("uTime", periodic);

    cube_attr->get_shader()->setVec3("uLight.position", glm::vec3(1.2f, 1.0f, 2.0f));
    cube_attr->get_shader()->setVec3("uLight.direction", glm::vec3(-1.2f, -1.0f, -2.0f));
    cube_attr->get_shader()->setVec3("uLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
    cube_attr->get_shader()->setVec3("uLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    cube_attr->get_shader()->setVec3("uLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    cube_attr->get_shader()->setFloat("uLight.atten_a", 0.0f);
    cube_attr->get_shader()->setFloat("uLight.atten_b", 0.0f);
    cube_attr->get_shader()->setFloat("uLight.cone_a", glm::cos(glm::radians(10.0f)));
    cube_attr->get_shader()->setFloat("uLight.cone_b", glm::cos(glm::radians(15.0f)));

    cube_attr->render(tex_manager);

    glDepthFunc(GL_LEQUAL);

    auto model = glm::translate(glm::mat4(1.0f), cam_pos);

    skybox_attr->get_shader()->use();

    skybox_attr->get_shader()->setMat4("uModel", model);
    skybox_attr->get_shader()->setMat4("uView", view);
    skybox_attr->get_shader()->setMat4("uPerspect", perspect);

    skybox_attr->render(tex_manager);

    glDepthFunc(GL_LESS);

    frame_buffer->unbind();

    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    screen_attr->get_shader()->use();
    screen_attr->render(tex_manager);

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
        cam_pos += delta_value * cam_front * 2.0f;
    if (key == 's')
        cam_pos -= delta_value * cam_front * 2.0f;
    if (key == 'a')
        cam_pos += delta_value * cam_links * 2.0f;
    if (key == 'd')
        cam_pos -= delta_value * cam_links * 2.0f;
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
    glutInitWindowSize(screen_w, screen_h);
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