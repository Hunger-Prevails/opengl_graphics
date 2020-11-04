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

TexManager::TexManager(bool y_flip) {
	unitCount = 0;

	format[1] = GL_RED;
	format[3] = GL_RGB;
	format[4] = GL_RGBA;

	stbi_set_flip_vertically_on_load(y_flip);
}

void TexManager::clear() {
	unitCount = 0;
}

void TexManager::load_texture(string filepath, string name) {

	if (texIDs.find(filepath) != texIDs.end()) {

		assert(texNames[filepath] == name);
		return;
	}
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width;
	int height;
	int n_channels;

	auto *data = stbi_load(filepath.c_str(), &width, &height, &n_channels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, format[n_channels], width, height, 0, format[n_channels], GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	texIDs[filepath] = texture;
	texNames[filepath] = name;
	texTargets[filepath] = GL_TEXTURE_2D;

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TexManager::load_skybox(string folder, string name) {

	if (texIDs.find(folder) != texIDs.end()) {

		assert(texNames[folder] == name);
		return;
	}
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int width;
	int height;
	int n_channels;

	for (unsigned int i = 0; i < 6; i ++)
	{
		auto filepath = folder + '/' + to_string(i) + ".jpg";

		auto tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

		auto *data = stbi_load(filepath.c_str(), &width, &height, &n_channels, 0);

		if (!data) {

			cout << "Skybox Load Fails: " << filepath << endl;
			exit(0);
		}
		glTexImage2D(tex_target, 0, format[n_channels], width, height, 0, format[n_channels], GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	texIDs[folder] = texture;
	texNames[folder] = name;
	texTargets[folder] = GL_TEXTURE_CUBE_MAP;

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void TexManager::add_texture(string fake_path, unsigned int buffer, string name) {

	if (texIDs.find(fake_path) != texIDs.end()) {

		assert(texNames[fake_path] == name);
		return;
	}
	texIDs[fake_path] = buffer;
	texNames[fake_path] = name;
	texTargets[fake_path] = GL_TEXTURE_2D;
}

void TexManager::upload(unsigned int program, string filepath) {

	if (texIDs.find(filepath) == texIDs.end()) {

		cout << "Unknown Texture Path: " << filepath << endl;
		exit(0);
	}
	glActiveTexture(GL_TEXTURE0 + unitCount);
	glBindTexture(texTargets[filepath], texIDs[filepath]);

	int sampler_uniform = glGetUniformLocation(program, texNames[filepath].c_str());
	glUniform1i(sampler_uniform, unitCount ++);
}