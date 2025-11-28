#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <cassert>

class Shader;

class TexManager
{
private:
	int unitCount;
public:
	std::map<int, unsigned int> format;
	std::map<std::string, unsigned int> texIDs;
	std::map<std::string, std::string> texNames;
	std::map<std::string, unsigned int> texTargets;
public:
	TexManager(bool y_flip = false);

	void clear();

	void load_texture(std::string filepath, std::string name);

	void load_skybox(std::string folder, std::string name);

	void add_texture(std::string fake_path, unsigned int buffer, std::string name);

	void upload(std::shared_ptr<Shader> shader, std::string filepath);
};
