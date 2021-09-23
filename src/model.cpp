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
#include <iomanip>
#include <map>
#include <vector>
#include <cassert>

#include "model.h"

Model::Model(string path, unsigned int program, bool y_flip)
{
    std::ifstream fin("res/tex_names.json", std::ifstream::binary);

    fin >> texNames;
    fin.close();

    this->program = program;
    tex_manager = new TexManager(y_flip);
    loadModel(path);
}

void Model::Draw(unsigned int program)
{
    for(auto &mesh:meshes) mesh.Draw(program, tex_manager);
}

void Model::loadModel(string path)
{
    Assimp::Importer importer;
    auto *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    this->root = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i ++) meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));

    for (auto &mesh:meshes) mesh.setupMesh(this->program);

    for (unsigned int i = 0; i < node->mNumChildren; i ++) processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i ++)
    {
        Vertex vertex;

        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals()) vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        assert(mesh->mTextureCoords[0]);
        
        auto *texCoord = mesh->mTextureCoords[0];

        vertex.TexCoord = glm::vec2(texCoord[i].x, texCoord[i].y);

        vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

        vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

        vertices.push_back(vertex);
    }
    vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumFaces; i ++)
    {
        auto face = mesh->mFaces[i];

        for(unsigned int j = 0; j < face.mNumIndices; j ++) indices.push_back(face.mIndices[j]);
    }
    auto* material = scene->mMaterials[mesh->mMaterialIndex];

    return Mesh(vertices, indices, loadMat(material));
}

vector<string> Model::loadMat(aiMaterial *mat)
{
    vector<string> texPaths;

    for (auto it = texNames.begin(); it != texNames.end(); it ++) {

        if (mat->GetTextureCount(it.value())) {

            aiString str;
            mat->GetTexture(it.value(), 0, &str);

            auto texPath = this->root + '/' + string(str.C_Str());
            tex_manager->load_texture(texPath, it.key());

            texPaths.push_back(texPath);
        }
    }
    return texPaths;
}