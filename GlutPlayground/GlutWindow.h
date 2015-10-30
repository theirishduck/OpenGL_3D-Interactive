#pragma once

#include "global.h"
#include "GLScene.h"

class GlutWindow
{
public:
	GlutWindow();
	~GlutWindow();

protected:
	GlutWindowDescriptor m_gd;
	GLScene *m_scene;
	int m_width;
	int m_height;
public:
	virtual int Draw();
	virtual int Reshape(int w, int h);
	virtual int KeyboardHandler(unsigned char key, int x, int y) = 0;
	virtual int SpecialKeyHandler(int key, int x, int y) = 0;
	virtual int MouseHandler(int button, int state, int x, int y) = 0;
	virtual int MotionHandler(int x, int y) = 0;
	virtual int PassiveMotionHandler(int x, int y) = 0;
	
};

