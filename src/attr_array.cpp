#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <utility>

#include "attr_array.h"
#include "shader.h"
#include "tex_manager.h"

using namespace std;

AttrArray::AttrArray(vector<float> attributes, vector<pair<string, size_t> > sizes, Shader *shader)
{
    this->shader = shader;

    size_t vertex_size = 0;

    for (auto it = sizes.begin(); it != sizes.end(); it ++) vertex_size += it->second;

    n_vertices = attributes.size() / vertex_size;

    size_t offset = 0;

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(float), attributes.data(), GL_STATIC_DRAW);

    for (auto it = sizes.begin(); it != sizes.end(); it ++) {

        auto attr_loc = shader->get_attr_location(it->first);

        glVertexAttribPointer(attr_loc, it->second, GL_FLOAT, false, vertex_size * sizeof(float), (void *)(offset * sizeof(float)));

        glEnableVertexAttribArray(attr_loc);

        offset += it->second;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->instances = 1;
}

Shader* AttrArray::get_shader()
{
    return this->shader;
}

void AttrArray::add_tex_path(string path)
{
    tex_paths.push_back(path);
}

void AttrArray::render(TexManager *tex_manager)
{
    tex_manager->clear();
    for (auto &path:tex_paths) tex_manager->upload(this->shader, path);

    glBindVertexArray(this->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, this->n_vertices, this->instances);
    glBindVertexArray(0);
}

void AttrArray::set_inst_mat4(vector<glm::mat4> attributes, string name)
{
    this->instances = attributes.size();

    unsigned int inst_vbo;
    glGenBuffers(1, &inst_vbo);

    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, inst_vbo);
    glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(glm::mat4), attributes.data(), GL_STATIC_DRAW);

    auto attr_loc = this->shader->get_attr_location(name);

    for (size_t i = 0; i < 4; i ++) {

        glEnableVertexAttribArray(attr_loc + i);

        glVertexAttribPointer(attr_loc + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(i * sizeof(glm::vec4)));

        glVertexAttribDivisor(attr_loc + i, 1);
    }
    glBindVertexArray(0);
}
