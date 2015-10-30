#pragma once

#include "GlutSubWindow.h"
#include "global.h"

class GlutSubDepthWindow
	: public GlutSubWindow
{
public:
	GlutSubDepthWindow(GlutWindowDescriptor parentGd, GlutWindow *refWindow);
	~GlutSubDepthWindow();

	int Display();
	int Reshape(int w, int h);
	int KeyboardHandler(unsigned char key, int x, int y);
	int SpecialKeyHandler(int key, int x, int y);
	int MouseHandler(int button, int state, int x, int y);
	int MouseWheelHandler(int wheel, int direction, int x, int y);
	int MotionHandler(int x, int y);
	int PassiveMotionHandler(int x, int y);
private:
	GlutWindow *m_refWindow;
	GLfloat *m_buffer;
	GLfloat *m_displayBuffer;

	void ReadBuffer();
	void WriteBuffer();
};

