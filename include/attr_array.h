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

class Shader;
class TexManager;

class AttrArray {

    unsigned int vao;
    unsigned int vbo;

    size_t n_vertices;

    Shader *shader;

    std::vector<std::string> tex_paths;

public:
    AttrArray(std::vector<float> attributes, std::vector<std::pair<std::string, size_t> > sizes, Shader *shader);

    Shader *get_shader();

    void add_tex_path(std::string path);

    void render(TexManager *tex_manager);

    void setup_instance_array(std::vector<float> instances, std::vector<std::pair<std::string, size_t> > sizes);
};
