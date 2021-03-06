#pragma once

#include <iostream>
#include <map>
#include <GL\freeglut.h>
#include "global.h"

using namespace std;

typedef map<GlutWindowDescriptor, map<string, GLuint> > GlobalTexturePool;
typedef map<string, GLuint> WindowTexturePool;

namespace FileUtil
{
	enum GLIOException
	{
		IMAGE_LOAD_ERROR
	};

	string ChooseFile();
	GLuint LoadTextureFromFile(const char *filename);

	static GlobalTexturePool g_texturePool;
}

