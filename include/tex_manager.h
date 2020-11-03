#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <map>
#include <string>
#include <cassert>

using namespace std;

class TexManager
{
private:
	int unitCount;
public:
	map<int, unsigned int> format;
	map<string, unsigned int> texIDs;
	map<string, string> texNames;
public:
	TexManager();

	void clear();

	void load_texture(string filepath, string name);

	void add_texture(string fake_path, unsigned int buffer, string name);

	void upload(GLuint program, string filepath);
};