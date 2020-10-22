#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "tex_manager.h"

using namespace std;

struct Vertex {

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh {

public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<string> texPaths;

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

public:
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<string> texPaths);

    void Draw(unsigned int program, TexManager *tex_manager);

    void setupMesh(unsigned int program);
};
