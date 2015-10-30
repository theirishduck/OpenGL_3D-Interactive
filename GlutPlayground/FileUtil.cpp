#include <GL\glew.h>
#include "FileUtil.h"
#include <Windows.h>
#include <Commdlg.h>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include "stdio.h"

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

GLuint FileUtil::LoadTextureFromFile(const char *filename)
{
	GLuint texture;
	IplImage *pImage = cvLoadImage(filename);
	cvFlip(pImage);
	if (!pImage) {
		fprintf(stderr, "PhotoScene::PhotoScene(): cannot load image %s\n", filename);
		throw - 1;
	}

	glGenTextures(1, &texture);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
		pImage->width, pImage->height, 0, GL_BGR, GL_UNSIGNED_BYTE, pImage->imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glDisable(GL_TEXTURE_2D);

	return texture;
}