#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cassert>

#include "mesh.h"
#include "json.hpp"

using namespace std;

class Model
{
    Shader *shader;

    TexManager *tex_manager;

    nlohmann::json texNames;

    string root;

    vector<Mesh> meshes;

    void load_model(string path);

    void process_node(aiNode *node, const aiScene *scene);

    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

    vector<string> load_material(aiMaterial *mat);

public:
    Model(string path, Shader *shader, bool y_flip = false);

    void render();

    Shader *get_shader();
};
