#include <iostream>
#include <GL\freeglut.h>
#include "GlutMainWindow.h"

GlutMainWindow *GlutWindow::CreateGlutMainWindow(int x, int y, int w, int h, int nRows, int nCols)
{
	GlutMainWindow *mainWindowPtr = new GlutMainWindow(x, y, w, h, nRows, nCols);
	if (mainWindowPtr == NULL)
	{
		std::cerr << "GlutWindow::CreateGlutMainWindow(): error, no available memory." << std::endl;
		return NULL;
	}
	else if(mainWindowPtr->m_gd < 0 || mainWindowPtr->m_gd >= MAX_GLUTWINDOW)
	{
		std::cerr << "GlutWindow::CreateGlutMainWindow(): error, GlutWindow pool has already been full." << std::endl;
		return NULL;
	}
	else
	{
		GlutWindow::g_glutWindowPool[mainWindowPtr->m_gd] = mainWindowPtr;
		GlutWindow::g_enable[mainWindowPtr->m_gd] = true;
		return mainWindowPtr;
	}
}

GlutMainWindow::GlutMainWindow(int x, int y, int w, int h, int nRows, int nCols) :
	GlutWindow(w, h), 
	m_nRows(nRows), 
	m_nCols(nCols)
{
	Init(x, y, w, h, nRows, nCols);
}

GlutMainWindow::~GlutMainWindow()
{
}

int GlutMainWindow::Layout(int w, int h)
{
	GlutWindowDescriptor curGd = glutGetWindow();

	int wStep = w / m_nCols;
	int hStep = h / m_nRows;
	
	std::vector<GlutSubWindow*>::iterator it = m_subwindows.begin();

	for (int i = 0; i < m_nRows; i++)
	{
		for (int j = 0; j < m_nCols; j++)
		{
			if (it != m_subwindows.end() && *it != NULL)
			{
				glutSetWindow((*it)->m_gd);
				glutPositionWindow(wStep * j, hStep * i);
				glutReshapeWindow(wStep, hStep);
				it++;
			}
			else if (it == m_subwindows.end())
			{
				break; // Number of subwindow is less than cells of main window
			}
			else if ((*it) == NULL)
			{
				std::cerr << "GlutMainWindow::Layout(): there is a null pointer in subwindows, please check your code." << std::endl;
			}
		}
	}

	if (m_subwindows.size() > m_nRows * m_nCols)
	{
		std::cout << "GlutMainWindow::Layout(): number of subwindows has exceed the number of cells of main window." 
			<< std::endl;
	}

	glutSetWindow(curGd);
	
	return 0;
}

int GlutMainWindow::GetIndex(int row, int col)
{
	int index;
	if (row < 0 || row >= m_nRows
		|| col < 0 || col >= m_nCols
		|| (index = m_nCols * row + col) >= m_subwindows.size())
	{
		return -1;
	}

	return index;
}

void GlutMainWindow::SetFullScreen()
{
	GlutWindowDescriptor curGd = glutGetWindow();
	glutSetWindow(m_gd);
	glutFullScreen();
	glutSetWindow(curGd);
}

void GlutMainWindow::SetTimerFunc(TimerFunc timerFunc, double msec)
{
	GlutWindowDescriptor curGd = glutGetWindow();
	glutSetWindow(m_gd);
	glutTimerFunc(msec, timerFunc, m_gd);
	glutSetWindow(curGd);
}

GlutSubWindow *GlutMainWindow::GetSubWindow(int row, int col)
{
	int index = GetIndex(row, col);
	if (index < 0)
	{
		std::cerr << "GlutMainWindow::GetSubWindow(): attempt to access out of range data." << std::endl;
		return NULL;
	}

	return m_subwindows[index];
}

int GlutMainWindow::AddSubWindow(GlutSubWindow * subWindowPtr)
{
	if (subWindowPtr == NULL)
	{
		std::cerr << "GlutMainWindow::AddSubWindow: failed to add a sub-window, since it is a null pointer." << std::endl;
		return -1;
	}

	m_subwindows.push_back(subWindowPtr);
	Layout(m_width, m_height);

	return 0;
}

int GlutMainWindow::ReplaceSubWindow(GlutSubWindow * subWindowPtr, int row, int col)
{
	int index = GetIndex(row, col);
	if (index < 0)
	{
		std::cerr << "GlutMainWindow::ReplaceSubWindow(): attempt to replace out of range data." << std::endl;
		return -1;
	}
	
	// Free previous subwindow
	GlutWindow::DeleteGlutWindow(m_subwindows[index]);

	m_subwindows[index] = subWindowPtr;
	Layout(m_width, m_height);

	return 0;
}

int GlutMainWindow::Display()
{
	GlutWindowDescriptor curGd = glutGetWindow();
	for (std::vector<GlutSubWindow*>::iterator it = m_subwindows.begin(); it != m_subwindows.end(); it++)
	{
		if (*it != NULL)
		{
			glutSetWindow((*it)->m_gd);
			//glutPostRedisplay();
		}
		else
		{
			std::cerr << "GlutMainWindow::Display(): attempt to display null pointer." << std::endl;
		}
	}
	glutSetWindow(curGd);


	frame++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - lasttime > 1000) {
		printf("Window(%d): FPS:%4.2f\n",
			m_gd,
			frame*1000.0 / (time - lasttime));
		lasttime = time;
		frame = 0;
	}

	return 0;
}

int GlutMainWindow::Reshape(int w, int h)
{
	GlutWindow::Reshape(w, h);
	Layout(w, h);

	return 0;
}

int GlutMainWindow::KeyboardHandler(unsigned char key, int x, int y)
{
	return 0;
}

int GlutMainWindow::SpecialKeyHandler(int key, int x, int y)
{
	return 0;
}

int GlutMainWindow::MouseHandler(int button, int state, int x, int y)
{
	return 0;
}

int GlutMainWindow::MouseWheelHandler(int wheel, int direction, int x, int y)
{
	return 0;
}

int GlutMainWindow::MotionHandler(int x, int y)
{
	return 0;
}

int GlutMainWindow::PassiveMotionHandler(int x, int y)
{
	return 0;
}

void GlutMainWindow::MeasureMainWindow()
{
#ifdef _MEASUREMENT
	frame++;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - lasttime > 1000) {
		printf("Window(%d): FPS:%4.2f\n",
			m_gd,
			frame*1000.0 / (time - lasttime));
		lasttime = time;
		frame = 0;
	}
#endif
}

void GlutMainWindow::Init(int x, int y, int w, int h, int nRows, int nCols)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(x, y);
	glutInitWindowSize(w, h);

	m_gd = glutCreateWindow("Main");
	glutDisplayFunc(GlutWindow::GlutDisplay);
	glutReshapeFunc(GlutWindow::GlutReshape);
	glutMouseFunc(GlutWindow::GlutMouseHandler);
	glutMotionFunc(GlutWindow::GlutMotionHandler);
	glutPassiveMotionFunc(GlutWindow::GlutPassiveMotionHandler);
	glutMouseWheelFunc(GlutWindow::GlutMouseWheelHandler);
	glutSpecialFunc(GlutWindow::GlutSpecialKeyHandler);
}

