#include <GL\glew.h>
#include "FileUtil.h"
#include <Windows.h>
#include <Commdlg.h>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include "stdio.h"
#include "global.h"

string FileUtil::ChooseFile()
{
	OPENFILENAME ofn;
	char file[256];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = file;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(file);
	ofn.lpstrFilter = "All(*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		return string(ofn.lpstrFile);
	}
	else
		throw - 1;
}

/*
	
*/
GLuint FileUtil::LoadTextureFromFile(const char *filename)
{
	GlutWindowDescriptor gd = glutGetWindow();
	WindowTexturePool &windowTexturePool = g_texturePool[gd];
	string filename_str(filename);
	
	GLuint texture;
	if(windowTexturePool.find(filename_str) != windowTexturePool.end()) // if found
	{ 
		texture = windowTexturePool[filename_str];
		std::cout << "FileUtil::LoadTextureFromFile(): duplicate texture " << texture << " for window " << gd << std::endl;
		return texture;
	}
	else
	{
		IplImage *pImage = cvLoadImage(filename);
		cvFlip(pImage);
		if (!pImage) {
			fprintf(stderr, "FileUtil::LoadTextureFromFile(): cannot load image %s\n", filename);
			throw IMAGE_LOAD_ERROR;
		}

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
			pImage->width, pImage->height, 0, GL_BGR, GL_UNSIGNED_BYTE, pImage->imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		windowTexturePool[filename_str] = texture;
		
		cvReleaseImage(&pImage);

		std::cout << "FileUtil::LoadTextureFromFile(): load texture " << texture << " for window " << gd << std::endl;

		return texture;
	}
}