#pragma once

#include <iostream>
#include <GL\freeglut.h>

using namespace std;

namespace FileUtil
{
	string ChooseFile();
	GLuint LoadTextureFromFile(const char *filename);
}

