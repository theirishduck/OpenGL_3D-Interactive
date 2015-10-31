#include <GL\freeglut.h>
#include <iostream>
#include <cstdio>
#include "GlutWindow.h"

GlutWindow *GlutWindow::g_glutWindowPool[MAX_GLUTWINDOW] = { 0 };

GlutWindow *GlutWindow::GetGlutWindow(GlutWindowDescriptor gd)
{
	if (!IsValid(gd))
	{
		std::cerr << "GlutWindow::GetGlutWindow(): invalid access at GlutWindowPool." << std::endl;
		return NULL;
	}

	return g_glutWindowPool[gd];
}

bool GlutWindow::IsValid(GlutWindowDescriptor gd)
{
	return (gd >= 0 && gd < MAX_GLUTWINDOW);
}

void GlutWindow::GlutDisplay()
{
	GlutWindowDescriptor gd = glutGetWindow();
	GlutWindow *window = GlutWindow::g_glutWindowPool[gd];
	glutPostRedisplay(); // Continuous re-display instead of evnet driven re-display
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
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->KeyboardHandler(key, x, y);
	}
}

void GlutWindow::GlutSpecialKeyHandler(int key, int x, int y)
{
	printf("GlutWindow::GlutSpecialKeyHandler(): %d\n", glutGetWindow());

	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->SpecialKeyHandler(key, x, y);
	}
}

void GlutWindow::GlutMouseHandler(int button, int state, int x, int y)
{
	printf("GlutWindow::GlutMouseHandler(): %d\n",glutGetWindow());

	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->MouseHandler(button, state, x, y);
	}
}

void GlutWindow::GlutMouseWheelHandler(int wheel, int direction, int x, int y)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->MouseWheelHandler(wheel, direction, x, y);
	}
}

void GlutWindow::GlutMotionHandler(int x, int y)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->MotionHandler( x, y);
	}
}

void GlutWindow::GlutPassiveMotionHandler(int x, int y)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->PassiveMotionHandler(x, y);
	}
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