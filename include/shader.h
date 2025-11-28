#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
	unsigned int program;

	std::vector<unsigned int> shaders;

	char *load_shader(std::string filepath);

	unsigned int compile_shader(char *segment, GLenum type);

public:
	void load_shader(std::string filepath, GLenum type);

	void link();

	void use();

	int get_attr_location(std::string attr_name);

	int get_uniform_location(std::string uniform_name);

	void setBool(const std::string &name, bool value);

	void setInt(const std::string &name, int value);

	void setFloat(const std::string &name, float value);

	void setVec2(const std::string &name, const glm::vec2 &value);

	void setVec2(const std::string &name, float x, float y);

	void setVec3(const std::string &name, const glm::vec3 &value);

	void setVec3(const std::string &name, float x, float y, float z);

	void setVec4(const std::string &name, const glm::vec4 &value);

	void setVec4(const std::string &name, float x, float y, float z, float w);

	void setMat2(const std::string &name, const glm::mat2 &mat);

	void setMat3(const std::string &name, const glm::mat3 &mat);

	void setMat4(const std::string &name, const glm::mat4 &mat);
};