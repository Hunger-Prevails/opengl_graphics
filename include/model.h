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

using namespace std;

class Model
{
public:
    map<aiTextureType, string> texNames;
    TexManager *tex_manager;
    vector<Mesh> meshes;
    string root;
    unsigned int program;

    Model(string path, unsigned int program, bool y_flip = false);

    void Draw(unsigned int program);
    
private:
    void loadModel(string path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    vector<string> loadMat(aiMaterial *mat);
};
