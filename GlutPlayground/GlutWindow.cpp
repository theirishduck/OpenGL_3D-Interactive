#include <GL\freeglut.h>
#include <iostream>
#include <cstdio>
#include "FileUtil.h"
#include "GlutWindow.h"

/**
	Store all GlutWindow instance
*/
GlutWindow *GlutWindow::g_glutWindowPool[MAX_GLUTWINDOW] = { 0 };

/**
	Store status of all GlutWindow.
	If g_enable[i] is false and g_glutWindowPool[i] is not null, destroy window before display.
*/
bool GlutWindow::g_enable[MAX_GLUTWINDOW] = { 0 };

/**
	A safer approach to access GlutWindow instance.
	Use this function to access GlutWindow instance in your code.
	Never directly access g_glutWindowPool.

	@param gd index in g_glutWindowPool
*/
GlutWindow *GlutWindow::GetGlutWindow(GlutWindowDescriptor gd)
{
	if (!IsValid(gd))
	{
		std::cerr << "GlutWindow::GetGlutWindow(): invalid access at GlutWindowPool." << std::endl;
		return NULL;
	}

	return g_glutWindowPool[gd];
}

/**
	Check gd is in the range if g_glutWindowPool

	@param gd index in g_glutWindowPool
*/
bool GlutWindow::IsValid(GlutWindowDescriptor gd)
{
	return (gd >= 0 && gd < MAX_GLUTWINDOW);
}

/**
	Mark g_enable of passing

	@param window GlutWindow instance we want to destroy
*/
void GlutWindow::DeleteGlutWindow(GlutWindow *window)
{
	if (window != NULL)
	{
		GlutWindowDescriptor gd = window->m_gd;
		if (IsValid(gd))
		{
			g_enable[gd] = false; // disable it. deletetion will be at next display
		}
	}
	else
	{
		std::cerr << "GlutWindow::DeleteGlutWindow(): attempt to delete a null window." << std::endl;
	}
}

/**
	Function pointer friendly function.
	We set this for all glutDisplayFunc of all windows
*/
void GlutWindow::GlutDisplay()
{
	GlutWindowDescriptor gd = glutGetWindow();
	GlutWindow *window = GlutWindow::GetGlutWindow(gd);
	if(window != NULL && GlutWindow::g_enable[gd])
	{
		window->Display();
		glutPostRedisplay(); // Continuous re-display instead of evnet driven re-display
	}
	else if(!GlutWindow::g_enable[gd])
	{
		delete window;
		g_glutWindowPool[gd] = NULL;
	}

}

/**
	Function pointer friendly function.
	We set this for all glutReshape of all windows
*/
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

/**
	Function pointer friendly function.
	We set this for all glutKeyboardFunc of all windows
*/
void GlutWindow::GlutKeyboardHandler(unsigned char key, int x, int y)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->KeyboardHandler(key, x, y);
	}
}

/**
	Function pointer friendly function.
	We set this for all glutSpecialKeyFunc of all windows
*/
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

/**
	Function pointer friendly function.
	We set this for all glutMouseFunc of all windows
*/
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

/**
	Function pointer friendly function.
	We set this for all glutMouseWheelFunc of all windows
*/
void GlutWindow::GlutMouseWheelHandler(int wheel, int direction, int x, int y)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->MouseWheelHandler(wheel, direction, x, y);
	}
}

/**
	Function pointer friendly function.
	We set this for all glutMotionFunc of all windows
*/
void GlutWindow::GlutMotionHandler(int x, int y)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	GlutWindow *window;
	if (IsValid(curGd) && (window = GetGlutWindow(curGd)) != NULL)
	{
		window->MotionHandler( x, y);
	}
}

/**
	Function pointer friendly function.
	We set this for all glutPassiveMotionFunc of all windows
*/
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
	frame = 0;
	lasttime = glutGet(GLUT_ELAPSED_TIME);
}

GlutWindow::~GlutWindow()
{
	glutDestroyWindow(m_gd);
}

/**
	Default Reshape function for all GlutWindows
	
	@param w width
	@param h height
*/
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