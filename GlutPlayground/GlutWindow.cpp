#include <GL\freeglut.h>
#include <iostream>
#include "GlutWindow.h"

GlutWindow *GlutWindow::g_glutWindowPool[MAX_GLUTWINDOW] = { 0 };

void GlutWindow::GlutDisplay()
{
	GlutWindowDescriptor gd = glutGetWindow();
	GlutWindow *window = GlutWindow::g_glutWindowPool[gd];
	window->Display();
}

void GlutWindow::GlutReshape(int w, int h)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	if (g_glutWindowPool[curGd] == NULL)
	{
		std::cerr << "GlutWindow::GlutReshape(): Window " << curGd << " doesn't exist." << std::endl;
		return;
	}
	else
	{
		GlutWindow::g_glutWindowPool[curGd]->Reshape(w, h);
	}
}

void GlutWindow::GlutKeyboardHandler(unsigned char key, int x, int y)
{

}

void GlutWindow::GlutSpecialKeyHandler(int key, int x, int y)
{

}

void GlutWindow::GlutMouseHandler(int button, int state, int x, int y)
{

}

void GlutWindow::GlutMouseWheelHandler(int wheel, int direction, int x, int y)
{
}

void GlutWindow::GlutMotionHandler(int x, int y)
{

}

void GlutWindow::GlutPassiveMotionHandler(int x, int y)
{

}

GlutWindow::GlutWindow(int w, int h):
	m_width(w), m_height(h), m_gd(-1)
{

}

GlutWindow::~GlutWindow()
{
}

int GlutWindow::Reshape(int w, int h)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	glutSetWindow(m_gd);

	m_width = w;
	m_height = h;

	glViewport(0, 0, w, h);

	glutSetWindow(curGd);
	
	return 0;
}