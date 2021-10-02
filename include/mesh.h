#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

class Shader;
class TexManager;

struct Vertex {

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh {

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::string> texPaths;

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::string> texPaths);

    void render(Shader *shader, TexManager *tex_manager);

    void setup_mesh(Shader *shader);
};
