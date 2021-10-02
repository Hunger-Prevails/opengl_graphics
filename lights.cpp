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

#include "frame_buffer.h"
#include "shader.h"
#include "tex_manager.h"
#include "data.h"

using namespace std;

unsigned int loadArrayBuffer(vector<float> attributes)
{
    unsigned int array_buffer;

    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(float), attributes.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return array_buffer;
}
int screen_w = 1200;
int screen_h = 800;

FrameBuffer *frame_buffer;
TexManager *tex_manager;

unsigned int cube_vao;
unsigned int screen_vao;
unsigned int skybox_vao;

Shader *cube_shader;
Shader *screen_shader;
Shader *skybox_shader;

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

    frame_buffer = new FrameBuffer(screen_w, screen_h);

    tex_manager = new TexManager();
    tex_manager->load_texture("res/diffuse.png", "uDiffuse");
    tex_manager->load_texture("res/specular.png", "uSpecular");

    tex_manager->add_texture("screen_frame", frame_buffer->get_buffer(), "uScreen");
    tex_manager->load_skybox("res/skybox", "uSkybox");

    vector<float> cube_data;
    cube_data.assign(cube_vertices, cube_vertices + 288);
    auto cube_buffer = loadArrayBuffer(cube_data);

    cube_shader = new Shader();

    cube_shader->load_shader("shaders/lights.vs", GL_VERTEX_SHADER);
    cube_shader->load_shader("shaders/lights.gs", GL_GEOMETRY_SHADER);
    cube_shader->load_shader("shaders/lights.fs", GL_FRAGMENT_SHADER);

    cube_shader->link();

    auto cube_a_position = cube_shader->get_attr_location("aPosition");
    auto cude_a_normal = cube_shader->get_attr_location("aNormal");
    auto cude_a_tex_coord = cube_shader->get_attr_location("aTexCoord");

    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);

    glBindBuffer(GL_ARRAY_BUFFER, cube_buffer);
    glVertexAttribPointer(cube_a_position, 3, GL_FLOAT, false, 8 * sizeof(float), (void *)0);
    glVertexAttribPointer(cude_a_normal, 3, GL_FLOAT, false, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(cude_a_tex_coord, 2, GL_FLOAT, false, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glEnableVertexAttribArray(cube_a_position);
    glEnableVertexAttribArray(cude_a_normal);
    glEnableVertexAttribArray(cude_a_tex_coord);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vector<float> screen_data;
    screen_data.assign(screen_vertices, screen_vertices + 12);
    auto screen_buffer = loadArrayBuffer(screen_data);

    screen_shader = new Shader();

    screen_shader->load_shader("shaders/screen.vs", GL_VERTEX_SHADER);
    screen_shader->load_shader("shaders/screen.fs", GL_FRAGMENT_SHADER);

    screen_shader->link();

    auto screen_a_position = screen_shader->get_attr_location("aPosition");

    glGenVertexArrays(1, &screen_vao);
    glBindVertexArray(screen_vao);

    glBindBuffer(GL_ARRAY_BUFFER, screen_buffer);
    glVertexAttribPointer(screen_a_position, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(screen_a_position);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vector<float> skybox_data;
    skybox_data.assign(skybox_vertices, skybox_vertices + 108);
    auto skybox_buffer = loadArrayBuffer(skybox_data);

    skybox_shader = new Shader();

    skybox_shader->load_shader("shaders/skybox.vs", GL_VERTEX_SHADER);
    skybox_shader->load_shader("shaders/skybox.fs", GL_FRAGMENT_SHADER);

    skybox_shader->link();

    auto skybox_a_position = skybox_shader->get_attr_location("aPosition");

    glGenVertexArrays(1, &skybox_vao);
    glBindVertexArray(skybox_vao);

    glBindBuffer(GL_ARRAY_BUFFER, skybox_buffer);
    glVertexAttribPointer(skybox_a_position, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(skybox_a_position);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    cube_shader->use();

    tex_manager->clear();
    tex_manager->upload(cube_shader, "res/diffuse.png");
    tex_manager->upload(cube_shader, "res/specular.png");
    tex_manager->upload(cube_shader, "res/skybox");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 perspect = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    auto view_x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    auto view_y = sin(glm::radians(pitch));
    auto view_z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cam_front = glm::normalize(glm::vec3(view_x, view_y, view_z));

    glm::mat4 view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_oben);

    cube_shader->setMat4("uModel", model);
    cube_shader->setMat4("uView", view);
    cube_shader->setMat4("uPerspect", perspect);
    cube_shader->setVec3("uCamPos", cam_pos);

    cube_shader->setFloat("uShininess", 64.0);
    cube_shader->setFloat("uTime", periodic);

    cube_shader->setVec3("uLight.position", glm::vec3(1.2f, 1.0f, 2.0f));
    cube_shader->setVec3("uLight.direction", glm::vec3(-1.2f, -1.0f, -2.0f));
    cube_shader->setVec3("uLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
    cube_shader->setVec3("uLight.diffuse", glm::vec3(0.6f, 0.6f, 0.6f));
    cube_shader->setVec3("uLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    cube_shader->setFloat("uLight.atten_a", 0.0f);
    cube_shader->setFloat("uLight.atten_b", 0.0f);
    cube_shader->setFloat("uLight.cone_a", glm::cos(glm::radians(10.0f)));
    cube_shader->setFloat("uLight.cone_b", glm::cos(glm::radians(15.0f)));

    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LEQUAL);

    skybox_shader->use();

    tex_manager->clear();
    tex_manager->upload(skybox_shader, "res/skybox");

    model = glm::translate(model, cam_pos);

    skybox_shader->setMat4("uModel", model);
    skybox_shader->setMat4("uView", view);
    skybox_shader->setMat4("uPerspect", perspect);

    glBindVertexArray(skybox_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);

    frame_buffer->unbind();

    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    screen_shader->use();

    tex_manager->clear();
    tex_manager->upload(screen_shader, "screen_frame");

    glBindVertexArray(screen_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

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