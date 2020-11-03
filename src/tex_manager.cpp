#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <map>
#include <string>
#include <cassert>

#include "tex_manager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TexManager::TexManager() {
	unitCount = 0;

	format[1] = GL_RED;
	format[3] = GL_RGB;
	format[4] = GL_RGBA;

	stbi_set_flip_vertically_on_load(true);
}

void TexManager::clear() {
	unitCount = 0;
}

void TexManager::load_texture(string filepath, string name) {

	if (texIDs.find(filepath) != texIDs.end()) {

		assert(texNames[filepath] == name);
		return;
	}
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width;
	int height;
	int n_channels;
	auto data = stbi_load(filepath.c_str(), &width, &height, &n_channels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, format[n_channels], width, height, 0, format[n_channels], GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	texIDs[filepath] = texture;
	texNames[filepath] = name;

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TexManager::add_texture(string fake_path, unsigned int buffer, string name) {

	if (texIDs.find(fake_path) != texIDs.end()) {

		assert(texNames[fake_path] == name);
		return;
	}
	texIDs[fake_path] = buffer;
	texNames[fake_path] = name;
}

void TexManager::upload(GLuint program, string filepath) {

	glActiveTexture(GL_TEXTURE0 + unitCount);
	glBindTexture(GL_TEXTURE_2D, texIDs[filepath]);

	int sampler_uniform = glGetUniformLocation(program, texNames[filepath].c_str());
	glUniform1i(sampler_uniform, unitCount ++);
}